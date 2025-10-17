//
// Created by 이채호 on 24. 10. 18.
//

#ifndef DYNAMIC_SERVER_IENDPOINTPROVIDER_H
#define DYNAMIC_SERVER_IENDPOINTPROVIDER_H

#include "IHttpServer.h"

class IEndpointProvider {
public:
    virtual void addEndpoint(IHttpServer& server) = 0;

    virtual ~IEndpointProvider() = default;
};

#endif //DYNAMIC_SERVER_IENDPOINTPROVIDER_H
