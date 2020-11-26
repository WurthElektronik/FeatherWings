![WE Logo](../../../../../assets/WE_Logo_small_t.png)

# Example: Amazon AWS with Calypso WiFi FeatherWing

## Introduction

This article describes how to create and manage IoT device using the [AWS Management Console](https://aws.amazon.com), how to connect to the cloud and visualize your data.

To use the steps in this tutorial, you need an [AWS subscription](https://azure.microsoft.com/en-us/free/). If you don't have an Azure subscription, please [create a free account](https://docs.aws.amazon.com/iot/latest/developerguide/setting-up.html) before you begin. Feel free to use free support plan

![Free AWS support plan](assets/select-support-plan.png)


## IoT-Core

In this tutorial, you'll install the software and create the AWS IoT resources necessary to connect a device to AWS IoT so that it can send and receive MQTT messages with AWS IoT Core. You'll see the messages in the MQTT client in the AWS IoT console. 

1. Sign in to the [AWS Management Console](https://aws.amazon.com).
2. Select **IoT Core** from the Menu.
![New IoT Core](assets/aws-iot-core-new.png)
3. Create new **Thing** (device) from the AWS IoT **Manage** menu and click **Register a thing** to start the process
![Iot Core new thing](assets/aws-iot-core-new-thing.png)
4. In **Add your device to the thing registry**  give Name to your thing and chose **Thing Type** if any exists.
![IoT Core new thing with type](assets/new-thing-registry.png)
If you do not have type, please create one with the **Create a type** button. Fill the form and press **Create thing type** button
![Create AWS IoT Thing Type](assets/create-thing-type.png)
5. When offered to use certificates please use **One-click certificate creation (recommended)** option and press the **Create certificate** button.
![Create certificate](assets/create-certificate.png)
6. If the certificates are created correctly, the success page will appear with the links to the certificates. 
![Certificate created successfully](assets/create-certificated-successfully.png)
**Download**: 
* **A certificate for this thing** and 
* **A private key**. 
Neither Public key nor root CA for AWS are needed since the root certificate is already integrated in Calypso module.
7. **Activate**


AWS-IoT
1. in Manage -> Thing*s - Click *Create button to create a new Thing
2. Create a single thing
3. In Add your device to the thing registry, give Name to the thing and chose Thing Type if any exists.
It there is no type, create one using Create a type button and then click Next
4. One-click certificate creation (recommended) Create certificate button
5. Download a certificate for this thing and download private key. Public key is not needed since the root certificate is already integrated in Calypso module.
6. Press button **Activate** and then **Attach a policy**
7. Click on **Create new policy**
![Add authorization to certificate](assets/add-authorization-to-certificate.png)
8. On the **Create a policy** page enter policy **Name** and **Add statement**.  In the **Action list** chose appropriate statements:
   * *iot:Connect*, 
   * *iot:Publish* 
   * *iot:Receive* 
  from the drop down menu. Chose **Effect** **Allow** and press **Create** or use advanced and copy the following code.

![Create policy](assets/create-policy.png)

```json

{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": "arn:aws:iot:eu-central-1:123456789012:client/we-iot-device-t1"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Publish",
      "Resource": "arn:aws:iot:eu-central-1:123456789012:topic/test"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Receive",
      "Resource": "arn:aws:iot:eu-central-1:123456789012:topic/test"
    }
  ]
}
```
9. Chose **Attach policy** from the **Actions** drop down menu and chose policy
    ![Attach Policy](assets/attach-policy.png)
10. Chose policy and press **Add**
![Add policy](assets/attach-policy-add.png)

This way, a thing with the policy and private key is created, to connect with the [Calypso WiFi FeatherWing](https://github.com/WE-eiSmart/FeatherWings-Hardware/tree/main/CalypsoWiFiFeatherWing) directly.


## Calypso WiFi FeatherWing code

