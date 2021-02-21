lhcluster
=========
LHCluster for distributed request queuing and notifications

Overview
--------
LHCluster is a lightweight set of Request queuing/dispatching components built ontop of ZMQ in C++. It uses a broker based star topology to route requests from clients to workers with varying degrees of guaranteed delivery.

Components
----------
* Broker - routes Requests/Responses between Clients and Workers or enqueues Requests in-memory if no Workers are available to handle a Request.
* Client - sends Requests and retrieves Responses.
* ClientProxy - sits in between Clients and the Broker and acts as a buffer for Client's requests. The ClientProxy will communicate with the Broker to ensure eventual delivery of a Request to a Worker .
* Worker - processes Requests. Workers will dynamically register with a Broker as being able to handle one or more RequestType's.

Supported Features
------------------
* Supports either TCP/IPC communication through the underlying ZMQ sockets
* Fully asynchronous Client's suitable for use in low latency scenarios
* One-way and two-way Requests
* Application defined Request payload through ZMQ frames
* Various degrees of guaranteed delivery (Sent to Worker, Received by Worker)
* Requests can be uniquely identified to prevent double sending enqueued/in-progress requests to Worker's
* Responses can be redirected to another Client other than the Client where the Request originated from
* Heartbeating to detect Broker/Worker deaths and handle accordingly
* Configurable logging with boost.log
* Python module built ontop of the native C++ library with boost.python
* Protobuf for internal seriailzation

Future Features
---------------
* Broker to Broker Request routing to eliminate single points of failure
* Anonynous Requests which do not have a unique RequestType/RequestId
* Load Balancing between Workers in the Broker
* Connecting to multiple Brokers for automatic failover
* High water marks, low water marks
* Capping the Number of Outstanding Requests in Broker to Prevent Runaway Memory Usage

Unsupported Features
--------------------
* Request persistence between process restarts - Requests are ONLY held in memory and are not persisted to any data store

Requests
--------
Requests in LHCluster are uniquely identified by a pair of RequestType and RequestId and contain an application defined payload transmitted in ZMQ frames. Requests in LHCluster are unique and will not result in duplicate work if a Request with the same RequestType/RequestId is either in progress or pending.

Additional Components
---------------------
In addition to the core Client/Broker/Worker components, there are analogous components for publishing notifications also built ontop of ZMQ
* NotifyPublisher - publish messages with a NotifyType, message will be fanned out to all subscribers of that NotifyType
* NotifyBroker - thin wrapper around ZMQ zproxy to switch messages between publishers and subscribers
* NotifySubscriber - subscribe to and process published messages for one or more NotifyType's

Building
--------
The lhcluster project will produce the following libraries
* liblhcluster - C++ library providing core components
* libpylhcluster - C++ library providing Python wrappers around liblhcluster
* pylhcluster - Python module leveraging liblhcluster and libpylhcluster

For the individual steps required to build the libraries and a listing of the various dependencies see the Dockerfile.

Dockerfile
----------
The Dockerfile is based on centos7 and will build the entire project (and any dependencies not provided by centos/epel) from source. When built directly it will produce countless images, two of which are useful. When built using the buildImage.sh script, it will produce two images:
* lhcluster:main - an image consisting of only runtime requirements
* lhcluster:test-env - an image consisting runtime requirements + build environment + build requirements

Examples/Usage
--------------
Demonstration of general Client/ClientProxy/Broker/Worker flow:
* C++ - liblhcluster/test/teste2eclientworker.cxx
* Python - pylhcluster/test/test_basic.py

Demonstration of NotifyPublisher/NotifyBroker/NotifySubscriber flow:
* C++ - liblhcluster/test/teste2enotifypubsub.cxx
* Python - pylhcluster/test/test_notify.py

Docker + Python Exmaples:
* Example using Clients/Workers pulled from a module installed in the base image - test/integration_notify
* Example using Clients/Workers pulled from a module sourced from a Docker volume - test/integration_simple_wpylocal

Benchmark
---------
A benchmark to give a better idea of throughput will be performed in the future.
