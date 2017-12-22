#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main(int argc, char **argv) {
  uWS::Hub h;

  PID pid;
  double Pinit = 0;
  double Iinit = 0;
  double Dinit = 0;

  //Initialize the pid coefficients.
  //Comments below outline the manual tuning process for the coefficients

  //pid.Init(0.2, 0.004, 3.0); // wobbly=>d coeff needs to be tuned, overshoots=>P coeff needs to be tuned

  //pid.Init(2, 0.004, 3.0); // start with increasing P coefficient // slightly better, still wobbly

  //pid.Init(1, 0.0000004, 10.0); // increase D, decrease I
  // works, runs though the track but VERY wobbly, I wouldnt want to drive in the car :)

  //pid.Init(1, 0.0000004, 40.0); // increase D, decrease I // runs off track

  //pid.Init(0.1, 0.00003, 0.3); // reduce P to avoid overshooting
  //pid.Init(0.2, 0, 0.3);       // much better, ignore I, no steering correction needed
  //pid.Init(0.2, 0, 3); /* works */

  // For faster iterative runs to tune parameters,
  // use command-line arguments to pass in coefficients
  if (argc == 4) {
      Pinit = atof(argv[1]);
      Iinit = atof(argv[2]);
      Dinit = atof(argv[3]);
  }
  else
  {
    // PID coefficents were set based on manual trial and error tuning
    Pinit = 0.2;
    Iinit = 0;
    Dinit = 3;
  }

  pid.Init(Pinit, Iinit, Dinit);

  h.onMessage([&pid](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;
          /*
          * Calculate steering value here, the steering value range is [-1, 1].
          */

          pid.UpdateError(cte);
          steer_value = pid.TotalError();

          // Clamp the steering angle value to be between [-1, 1]
          steer_value = (steer_value > 1.0) ? 1.0 : ((steer_value < -1.0) ? -1 : steer_value);

          // DEBUG
          std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;

          double throttle = 0.3;

          // Scale down throttle to slow down during turns to avoid driving off course
          if((speed > 15) && (fabs(steer_value)>0.1))
            throttle *= (1-speed/100);

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = throttle;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
