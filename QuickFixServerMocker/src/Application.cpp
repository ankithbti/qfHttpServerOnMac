/****************************************************************************
 ** Copyright (c) 2001-2014
 **
 ** This file is part of the QuickFIX FIX Engine
 **
 ** This file may be distributed under the terms of the quickfixengine.org
 ** license as defined by quickfixengine.org and appearing in the file
 ** LICENSE included in the packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.quickfixengine.org/LICENSE for licensing information.
 **
 ** Contact ask@quickfixengine.org if any conditions of this licensing are
 ** not clear to you.
 **
 ****************************************************************************/

#include <map>

#include "Application.h"
#include "quickfix/Session.h"

#include "quickfix/fix40/ExecutionReport.h"
#include "quickfix/fix41/ExecutionReport.h"
#include "quickfix/fix42/ExecutionReport.h"
#include "quickfix/fix43/ExecutionReport.h"
#include "quickfix/fix44/ExecutionReport.h"
#include "quickfix/fix50/ExecutionReport.h"
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>

namespace qfHttpServer {

    void Application::onCreate(const FIX::SessionID& sessionID) {
        std::ifstream fixMessageFileStream("/Users/ankithbti/qf_log/FIX.4.4-EXECUTOR-CLIENT1.messages.current.log");
        if (fixMessageFileStream.good()) {
            std::string line;
            while (std::getline(fixMessageFileStream, line)) {
                std::cout << line << std::endl;
                const boost::regex e("(([0-9]{8})-([0-9]{2}):([0-9]{2}):([0-9]{2}).([0-9]{3}) : (.*))");
                std::string sampleStr = "20150608-17:41:47.302 : 8=FIX.4.49";
                boost::smatch what;
                if (boost::regex_match (line, what, e)) {
                    //std::cout << " Matched " << std::endl;
                    std::string fixMessage = what[7];
                    std::cout << " Matched Fix: " << fixMessage <<std::endl;
                    FIX::Message msg(fixMessage, true);
                    std::cout << " Fix Msg : " << msg.toXML() << std::endl;
                } else {
                    std::cout << " Unmatched " << std::endl;
                }
            }
        }

    }

    void Application::onLogon(const FIX::SessionID& sessionID) {
        _sessionIds.insert(sessionID.toString());
        _sessionMap.insert(std::pair<std::string, FIX::Session*>(sessionID.toString(), _acceptor->getSession(sessionID)));
    }

    void Application::onLogout(const FIX::SessionID& sessionID) {
        _sessionMap.erase(sessionID.toString());
    }

    void Application::toAdmin(FIX::Message& message,
            const FIX::SessionID& sessionID) {
    }

    void Application::toApp(FIX::Message& message,
            const FIX::SessionID& sessionID)
    throw ( FIX::DoNotSend) {
    }

    void Application::fromAdmin(const FIX::Message& message,
            const FIX::SessionID& sessionID)
    throw ( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) {
    }

    void Application::fromApp(const FIX::Message& message,
            const FIX::SessionID& sessionID)
    throw ( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) {
        crack(message, sessionID);
    }

    void Application::onMessage(const FIX40::NewOrderSingle& message,
            const FIX::SessionID& sessionID) {
        FIX::Symbol symbol;
        FIX::Side side;
        FIX::OrdType ordType;
        FIX::OrderQty orderQty;
        FIX::Price price;
        FIX::ClOrdID clOrdID;
        FIX::Account account;

        message.get(ordType);

        if (ordType != FIX::OrdType_LIMIT)
            throw FIX::IncorrectTagValue(ordType.getField());

        message.get(symbol);
        message.get(side);
        message.get(orderQty);
        message.get(price);
        message.get(clOrdID);

        FIX40::ExecutionReport executionReport = FIX40::ExecutionReport
                (FIX::OrderID(genOrderID()),
                FIX::ExecID(genExecID()),
                FIX::ExecTransType(FIX::ExecTransType_NEW),
                FIX::OrdStatus(FIX::OrdStatus_FILLED),
                symbol,
                side,
                orderQty,
                FIX::LastShares(orderQty),
                FIX::LastPx(price),
                FIX::CumQty(orderQty),
                FIX::AvgPx(price));

        executionReport.set(clOrdID);

        if (message.isSet(account))
            executionReport.setField(message.get(account));

        try {
            FIX::Session::sendToTarget(executionReport, sessionID);
        } catch (FIX::SessionNotFound&) {
        }
    }

    void Application::onMessage(const FIX41::NewOrderSingle& message,
            const FIX::SessionID& sessionID) {
        FIX::Symbol symbol;
        FIX::Side side;
        FIX::OrdType ordType;
        FIX::OrderQty orderQty;
        FIX::Price price;
        FIX::ClOrdID clOrdID;
        FIX::Account account;

        message.get(ordType);

        if (ordType != FIX::OrdType_LIMIT)
            throw FIX::IncorrectTagValue(ordType.getField());

        message.get(symbol);
        message.get(side);
        message.get(orderQty);
        message.get(price);
        message.get(clOrdID);

        FIX41::ExecutionReport executionReport = FIX41::ExecutionReport
                (FIX::OrderID(genOrderID()),
                FIX::ExecID(genExecID()),
                FIX::ExecTransType(FIX::ExecTransType_NEW),
                FIX::ExecType(FIX::ExecType_FILL),
                FIX::OrdStatus(FIX::OrdStatus_FILLED),
                symbol,
                side,
                orderQty,
                FIX::LastShares(orderQty),
                FIX::LastPx(price),
                FIX::LeavesQty(0),
                FIX::CumQty(orderQty),
                FIX::AvgPx(price));

        executionReport.set(clOrdID);

        if (message.isSet(account))
            executionReport.setField(message.get(account));

        try {
            FIX::Session::sendToTarget(executionReport, sessionID);
        } catch (FIX::SessionNotFound&) {
        }
    }

    void Application::onMessage(const FIX42::NewOrderSingle& message,
            const FIX::SessionID& sessionID) {
        FIX::Symbol symbol;
        FIX::Side side;
        FIX::OrdType ordType;
        FIX::OrderQty orderQty;
        FIX::Price price;
        FIX::ClOrdID clOrdID;
        FIX::Account account;

        message.get(ordType);

        if (ordType != FIX::OrdType_LIMIT)
            throw FIX::IncorrectTagValue(ordType.getField());

        message.get(symbol);
        message.get(side);
        message.get(orderQty);
        message.get(price);
        message.get(clOrdID);

        FIX42::ExecutionReport executionReport = FIX42::ExecutionReport
                (FIX::OrderID(genOrderID()),
                FIX::ExecID(genExecID()),
                FIX::ExecTransType(FIX::ExecTransType_NEW),
                FIX::ExecType(FIX::ExecType_FILL),
                FIX::OrdStatus(FIX::OrdStatus_FILLED),
                symbol,
                side,
                FIX::LeavesQty(0),
                FIX::CumQty(orderQty),
                FIX::AvgPx(price));

        executionReport.set(clOrdID);
        executionReport.set(orderQty);
        executionReport.set(FIX::LastShares(orderQty));
        executionReport.set(FIX::LastPx(price));

        if (message.isSet(account))
            executionReport.setField(message.get(account));

        try {
            FIX::Session::sendToTarget(executionReport, sessionID);
        } catch (FIX::SessionNotFound&) {
        }
    }

    void Application::onMessage(const FIX43::NewOrderSingle& message,
            const FIX::SessionID& sessionID) {
        FIX::Symbol symbol;
        FIX::Side side;
        FIX::OrdType ordType;
        FIX::OrderQty orderQty;
        FIX::Price price;
        FIX::ClOrdID clOrdID;
        FIX::Account account;

        message.get(ordType);

        if (ordType != FIX::OrdType_LIMIT)
            throw FIX::IncorrectTagValue(ordType.getField());

        message.get(symbol);
        message.get(side);
        message.get(orderQty);
        message.get(price);
        message.get(clOrdID);

        FIX43::ExecutionReport executionReport = FIX43::ExecutionReport
                (FIX::OrderID(genOrderID()),
                FIX::ExecID(genExecID()),
                FIX::ExecType(FIX::ExecType_FILL),
                FIX::OrdStatus(FIX::OrdStatus_FILLED),
                side,
                FIX::LeavesQty(0),
                FIX::CumQty(orderQty),
                FIX::AvgPx(price));

        executionReport.set(clOrdID);
        executionReport.set(symbol);
        executionReport.set(orderQty);
        executionReport.set(FIX::LastQty(orderQty));
        executionReport.set(FIX::LastPx(price));

        if (message.isSet(account))
            executionReport.setField(message.get(account));

        try {
            FIX::Session::sendToTarget(executionReport, sessionID);
        } catch (FIX::SessionNotFound&) {
        }
    }

    void Application::onMessage(const FIX44::NewOrderSingle& message,
            const FIX::SessionID& sessionID) {
        FIX::Symbol symbol;
        FIX::Side side;
        FIX::OrdType ordType;
        FIX::OrderQty orderQty;
        FIX::Price price;
        FIX::ClOrdID clOrdID;
        FIX::Account account;

        message.get(ordType);

        if (ordType != FIX::OrdType_LIMIT)
            throw FIX::IncorrectTagValue(ordType.getField());

        message.get(symbol);
        message.get(side);
        message.get(orderQty);
        message.get(price);
        message.get(clOrdID);

        FIX44::ExecutionReport executionReport = FIX44::ExecutionReport
                (FIX::OrderID(genOrderID()),
                FIX::ExecID(genExecID()),
                FIX::ExecType(FIX::ExecType_FILL),
                FIX::OrdStatus(FIX::OrdStatus_FILLED),
                side,
                FIX::LeavesQty(0),
                FIX::CumQty(orderQty),
                FIX::AvgPx(price));

        executionReport.set(clOrdID);
        executionReport.set(symbol);
        executionReport.set(orderQty);
        executionReport.set(FIX::LastQty(orderQty));
        executionReport.set(FIX::LastPx(price));

        if (message.isSet(account))
            executionReport.setField(message.get(account));

        try {
            FIX::Session::sendToTarget(executionReport, sessionID);
        } catch (FIX::SessionNotFound&) {
        }
    }

    void Application::onMessage(const FIX50::NewOrderSingle& message,
            const FIX::SessionID& sessionID) {
        FIX::Symbol symbol;
        FIX::Side side;
        FIX::OrdType ordType;
        FIX::OrderQty orderQty;
        FIX::Price price;
        FIX::ClOrdID clOrdID;
        FIX::Account account;

        message.get(ordType);

        if (ordType != FIX::OrdType_LIMIT)
            throw FIX::IncorrectTagValue(ordType.getField());

        message.get(symbol);
        message.get(side);
        message.get(orderQty);
        message.get(price);
        message.get(clOrdID);

        FIX50::ExecutionReport executionReport = FIX50::ExecutionReport
                (FIX::OrderID(genOrderID()),
                FIX::ExecID(genExecID()),
                FIX::ExecType(FIX::ExecType_FILL),
                FIX::OrdStatus(FIX::OrdStatus_FILLED),
                side,
                FIX::LeavesQty(0),
                FIX::CumQty(orderQty));

        executionReport.set(clOrdID);
        executionReport.set(symbol);
        executionReport.set(orderQty);
        executionReport.set(FIX::LastQty(orderQty));
        executionReport.set(FIX::LastPx(price));
        executionReport.set(FIX::AvgPx(price));

        if (message.isSet(account))
            executionReport.setField(message.get(account));

        try {
            FIX::Session::sendToTarget(executionReport, sessionID);
        } catch (FIX::SessionNotFound&) {
        }
    }
}