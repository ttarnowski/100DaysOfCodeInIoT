#pragma once

#include <SingleHostHTTPSClient.hpp>

void getTemperature(SingleHostHTTPSClient *client,
                    std::function<void(const char *, float)> onLoad) {

  SingleHostHTTPSClient::Request req;
  req.path = "/dev/temperature";

  client->sendRequest(req, [onLoad](SingleHostHTTPSClient::Response response) {
    if (response.error != nullptr) {
      onLoad(response.error, 0);
      return;
    }

    double temp = strtod(response.body, NULL);
    onLoad(nullptr, (float)temp);
  });
}
