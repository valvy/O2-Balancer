#include "O2/Balancer/Devices/Connection.h"
#include "O2/Balancer/Devices/AbstractDevice.h"
#include "O2/Balancer/Exceptions/UnimplementedException.h"
#include "O2/Balancer/Utilities/DataTypes.h"
#include <boost/format.hpp>
#include <chrono>
#include <thread>
using namespace O2::Balancer;

Connection::Connection(const std::string& name, AbstractDevice* device){
    this->name = name;
    this->device = device;
    this->device->fChannels.insert(
        std::pair<std::string, std::vector<FairMQChannel>>(name, std::vector<FairMQChannel>()));
}

void Connection::updateAllRateLogging(const int& logg){
     for(auto& i : this->device->fChannels.at(name)){
        i.UpdateRateLogging(logg);
    }
}

void Connection::updateAllReceiveBuffer(const int& buffer){
    for(auto& i : this->device->fChannels.at(name)){
        i.UpdateRcvBufSize(buffer);
    }
}

void Connection::updateAllSendBuffer(const int& buffer){
    for(auto& i : this->device->fChannels.at(name)){
        i.UpdateSndBufSize(buffer);
    }
}

void Connection::updateAllSendKernelSize(const int& size){
    for(auto& i : this->device->fChannels.at(name)){
        i.UpdateSndKernelSize(size);
    }
}

void Connection::updateAllReceiveKernelSize(const int& size){
    for(auto& i : this->device->fChannels.at(name)){
        i.UpdateRcvKernelSize(size);
    }
}

void Connection::useClusterManager(std::function<void(std::shared_ptr<ClusterManager>)> cl){
    std::unique_lock<std::mutex> lck (this->device->zoolock);
    cl(this->device->clusterManager);
}

std::string Connection::typeToString(ConnectionType type) const{
    switch(type){
        case ConnectionType::Publish:
        return "pub";
        case ConnectionType::Pull:
        return "pull";
        case ConnectionType::Subscribe:
        return "sub";
        case ConnectionType::Push:
        return "push";
        default:
        throw Exceptions::UnimplementedException("typeToString doesn't implement this case");
    }
}

std::string Connection::methodToString(ConnectionMethod method) const{
    switch(method){
        case ConnectionMethod::Bind:
        return "bind";
        case ConnectionMethod::Connect:
        return "connect";
        default:
        throw Exceptions::UnimplementedException("MethodToString doesn't implement this case");
    }
}

std::shared_ptr<DeviceSetting> Connection::addInputChannel(ConnectionType type, ConnectionMethod method,const std::string& ip, int port){
    this->device->fChannels.at(name).push_back(FairMQChannel(
        this->typeToString(type),
        this->methodToString(method),
        "tcp://" + std::string(ip) + ":" + std::to_string(port)
    ));
    return std::shared_ptr<DeviceSetting>(new DeviceSetting(port,ip));
}

std::shared_ptr<DeviceSetting> Connection::addOutputChannel(ConnectionType type, ConnectionMethod method, const std::string& ip, int port){
    //Stop what you are doing, and wait until it's registered in ZooKeeper. In case of failure try again later  
    //const std::string name = this->name + ip + std::to_string(port); 
    while(!device->addHandle(this->name, O2::Balancer::DeviceSetting(port,ip))){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        LOG(INFO) << "Sleeping for next handle";
    }
    return this->addInputChannel(type,method,ip,port);
   
}


std::vector<FairMQChannel>& Connection::getChannels() const{
    return this->device->fChannels.at(this->getName());
}


size_t Connection::channelSize() const{
    return this->device->fChannels.at(this->getName()).size();
}

std::vector<std::string> Connection::getOfflineDevices(std::vector<DeviceSetting> nChannels){
    std::vector<std::string> results;
    for(size_t i = 0; i < this->device->fChannels.at(name).size(); i++){
        bool used = false;;
        const std::string oldIp = this->device->fChannels.at(name)[i].GetAddress();
        for(auto newDevice : nChannels){
            const std::string other = "tcp://" + newDevice.ip + ":" + std::to_string(newDevice.port);
            if(oldIp == other){
                used = true;
                break;
            }
        }
        if(!used){
            LOG(WARN) << boost::format("Device %s is offline, disabling channel") % oldIp;
            results.push_back(oldIp);
        }
    }
    return results;
}

void Connection::updateChannels(std::vector<DeviceSetting> nChannels){
    //First filter all the old ips out
    std::vector<std::string> deleteIp = this->getOfflineDevices(nChannels);
    this->device->fChannels.at(name).erase(
        std::remove_if(
            this->device->fChannels.at(name).begin(),
            this->device->fChannels.at(name).end(), 
            [&, deleteIp](FairMQChannel& e) -> bool{
                for(size_t i = 0; i < deleteIp.size(); i++ ){
                    if(deleteIp[i] == e.GetAddress()){
                        return true;
                    } 
                }
                return false;
            }
        )
    );
  
    this->device->fChannels.at(name).shrink_to_fit();
    LOG(WARN) << this->device->fChannels.at(name).size() << " remaining";

}





std::string Connection::getName() const{
    return this->name;
}