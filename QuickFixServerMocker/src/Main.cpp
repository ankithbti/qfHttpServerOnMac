
//#include <quickfix/Session.h>
#include <Add.hpp>
#include "quickfix/FileStore.h"
#include <quickfix/FileLog.h>
#include "quickfix/ThreadedSocketAcceptor.h"
#include "quickfix/Log.h"
#include "quickfix/SessionSettings.h"
#include <string>
#include <iostream>
#include <fstream>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <HttpServer.hpp>
#include "Application.h"
#include "QuickFixHttpServer.hpp"

int main(int argc, char** argv) {
    std::cout << " Test " << std::endl;
    Add a;
    a.add(5, 6);

    if (argc != 2) {
        std::cout << "usage: " << argv[ 0 ]
                << " FILE." << std::endl;
        return 0;
    }
    std::string file = argv[ 1 ];

    try {

        FIX::SessionSettings settings(file);
        qfHttpServer::Application application;
        FIX::FileStoreFactory storeFactory(settings);
        FIX::FileLogFactory logFactory(settings);
        boost::shared_ptr<FIX::ThreadedSocketAcceptor> acceptor(new FIX::ThreadedSocketAcceptor(application, storeFactory, settings, logFactory));
        application.setAcceptor(acceptor);
        acceptor->start();
        std::string port("1234");
        std::string docRoot("/Users/ankithbti/Development/Cpp/httpServer");
        httpServer::HttpReqHandlerInterface::SmartPtr reqHandler(new qfHttpServer::QuickFixHttpServer(docRoot, application));
        httpServer::HttpServer server(port, reqHandler);
        server.start();
        try {
            while (true) {
                boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
            }
        } catch (boost::thread_interrupted& err) {
            std::cout << " Thread Interrupted. " << std::endl;
        }
        acceptor->stop();
        server.stop();
        return 0;
    } catch (std::exception & e) {
        std::cout << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << " Exception ... " << std::endl;
    }

    return 0;
}
