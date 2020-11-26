![WE Logo](../../../../../assets/WE_Logo_small_t.png)

# Example: Microsoft Azure visualization with WebApp

## Introduction

In this example, you learn how to visualize real-time sensor data that your IoT hub receives with a node.js web app running on your local computer. After running the web app locally, you can optionally follow steps to host the web app in Azure App Service. If you want to try to visualize the data in your IoT hub by using Power BI or Excel please take a look at [real-time sensor data from Azure IoT Hub using Power BI or Excel](../) chapter.

## What you need

* Add a consumer group to your IoT hub that the web application will use to read sensor data
* Download the web app code from GitHub
* Examine the web app code
* Configure environment variables to hold the IoT Hub artifacts needed by your web app
* Run the web app on your development machine
* Open a web page to see real-time temperature and humidity data from your IoT hub
* (Optional) Use Azure CLI to host your web app in Azure App Service

## Necessary steps

* Complete the Sensor2Combo or one of the device tutorials. These cover the following requirements:
  * An active Azure subscription
  * An Iot hub under your subscription
  * A client application that sends messages to your Iot hub
* [Download Git](https://www.git-scm.com/downloads)
* The steps in this article assume a Windows development machine; however, you can easily perform these steps on a Linux system in your preferred shell.
* Use [Azure Cloud Shell](https://docs.microsoft.com/en-us/azure/cloud-shell/quickstart) using the bash environment.
[![Launch Cloud Shell](../assets/launchcloudshell.png)](https://shell.azure.com/)

* If you prefer, install the Azure CLI to run CLI reference commands.
  * If you're using a local install, sign in with Azure CLI by using the az login command. To finish the authentication process, follow the steps displayed in your terminal. See Sign in with Azure CLI for additional sign-in options.
  * When you're prompted, install Azure CLI extensions on first use. For more information about extensions, see Use extensions with Azure CLI.
  * Run ``az version`` to find the version and dependent libraries that are installed. To upgrade to the latest version, run ``az upgrade``.

## Create app parameters in Azure

### Add a consumer group to your IoT hub

[Consumer groups](https://docs.microsoft.com/en-us/azure/event-hubs/event-hubs-features#event-consumers) provide independent views into the event stream that enable apps and Azure services to independently consume data from the same Event Hub endpoint. In this section, you add a consumer group to your IoT hub's built-in endpoint that the web app will use to read data from.

Run the following command to add a consumer group to the built-in endpoint of your IoT hub:

```bash
az iot hub consumer-group create --hub-name YourIoTHubName --name YourConsumerGroupName
```

Note down the name you choose, you'll need it later in this tutorial.

### Get a service connection string for your IoT hub

IoT hubs are created with several default access policies. One such policy is the service policy, which provides sufficient permissions for a service to read and write the IoT hub's endpoints. Run the following command to get a connection string for your IoT hub that adheres to the service policy:

```bash
az iot hub show-connection-string --hub-name YourIotHub --policy-name service
```
The connection string should look similar to the following:

```JavaScript
"HostName={YourIotHubName}.azure-devices.net;SharedAccessKeyName=service;SharedAccessKey={YourSharedAccessKey}"
```

Note down the service connection string, you'll need it later in this tutorial.

### Download the web app from GitHub

Open web application source code in the [Visual Studio Code](code.visualstudio.com) and install **npm** if you do not have it.

