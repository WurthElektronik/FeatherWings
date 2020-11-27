/* eslint-disable max-classes-per-file */
/* eslint-disable no-restricted-globals */
/* eslint-disable no-undef */
$(document).ready(() => {
  // if deployed to a site supporting SSL, use wss://
  const protocol = document.location.protocol.startsWith('https') ? 'wss://' : 'ws://';
  const webSocket = new WebSocket(protocol + location.host);

  // A class for holding the last N points of telemetry for a device
  class DeviceData {
    constructor(deviceId) {
      this.deviceId = deviceId;
      this.maxLen = 50;
      this.timeData = new Array(this.maxLen);
      this.temperatureData = new Array(this.maxLen);
      this.pressureData = new Array(this.maxLen);
      this.humidityData = new Array(this.maxLen);
      this.accelerationXData = new Array(this.maxLen);
      this.accelerationYData = new Array(this.maxLen);
      this.accelerationZData = new Array(this.maxLen);
    }

    addData(time, temperature, pressure, temperature_p, humidity, temperature_h, accelerationX, accelerationY, accelerationZ) {
      //Temperature form the humidity and pressure sensor not used, since the Sensor FeatherFind contains separate temperature sensor
      this.timeData.push(time);
      this.temperatureData.push(temperature);
      this.pressureData.push(pressure || null);
      this.humidityData.push(humidity || null);
      this.accelerationXData.push(accelerationX || null);
      this.accelerationYData.push(accelerationY || null);
      this.accelerationZData.push(accelerationZ || null);

      if (this.timeData.length > this.maxLen) {
        this.timeData.shift();
        this.temperatureData.shift();
        this.pressureData.shift();
        this.humidityData.shift();
        this.accelerationXData.shift();
        this.accelerationYData.shift();
        this.accelerationZData.shift();
      }
    }
  }

  // All the devices in the list (those that have been sending telemetry)
  class TrackedDevices {
    constructor() {
      this.devices = [];
    }

    // Find a device based on its Id
    findDevice(deviceId) {
      for (let i = 0; i < this.devices.length; ++i) {
        if (this.devices[i].deviceId === deviceId) {
          return this.devices[i];
        }
      }

      return undefined;
    }

    getDevicesCount() {
      return this.devices.length;
    }
  }

  const trackedDevices = new TrackedDevices();

  // Define the chart axes
  //Temperature and humidity chart
  const temperatureChartData = {
    datasets: [
      {
        fill: false,
        label: 'Temperature',
        yAxisID: 'Temperature',
        borderColor: 'rgba(227, 0, 11, 1)',
        pointBoarderColor: 'rgba(227, 0, 11, 1)',
        backgroundColor: 'rgba(227, 0, 11, 0.4)',
        pointHoverBackgroundColor: 'rgba(227, 0, 11, 1)',
        pointHoverBorderColor: 'rgba(227, 0, 11, 1)',
        spanGaps: true,
      }
    ]
  };

  //Pressure chart
  const pressureChartData = {
    datasets: [
      {
        fill: false,
        label: 'Pressure',
        yAxisID: 'Pressure',
        borderColor: 'rgba(24, 120, 240, 1)',
        pointBoarderColor: 'rgba(24, 120, 240, 1)',
        backgroundColor: 'rgba(24, 120, 240, 0.4)',
        pointHoverBackgroundColor: 'rgba(24, 120, 240, 1)',
        pointHoverBorderColor: 'rgba(24, 120, 240, 1)',
        spanGaps: true,
      }
    ]
  };

  //Pressure chart
  const humidityChartData = {
    datasets: [
      {
        fill: false,
        label: 'Humidity',
        yAxisID: 'Humidity',
        borderColor: 'rgba(0, 79, 146, 1)',
        pointBoarderColor: 'rgba(0, 79, 146, 1)',
        backgroundColor: 'rgba(0, 79, 146, 0.4)',
        pointHoverBackgroundColor: 'rgba(0, 79, 146, 1)',
        pointHoverBorderColor: 'rgba(0, 79, 146, 1)',
        spanGaps: true,
      }
    ]
  };

  //Pressure chart
  const accelerationChartData = {
    datasets: [
      {
        fill: false,
        label: 'X-Axis',
        yAxisID: 'Acceleration',
        borderColor: 'rgba(0, 79, 146, 1)',
        pointBoarderColor: 'rgba(0, 79, 146, 1)',
        backgroundColor: 'rgba(0, 79, 146, 0.4)',
        pointHoverBackgroundColor: 'rgba(0, 79, 146, 1)',
        pointHoverBorderColor: 'rgba(0, 79, 146, 1)',
        spanGaps: true,
      },
      {
        fill: false,
        label: 'Y-Axis',
        yAxisID: 'Acceleration',
        borderColor: 'rgba(227, 0, 11, 1)',
        pointBoarderColor: 'rgba(227, 0, 11, 1)',
        backgroundColor: 'rgba(227, 0, 11, 0.4)',
        pointHoverBackgroundColor: 'rgba(227, 0, 11, 1)',
        pointHoverBorderColor: 'rgba(227, 0, 11, 1)',
        spanGaps: true,
      },
      {
        fill: false,
        label: 'Z-axis',
        yAxisID: 'Acceleration',
        borderColor: 'rgba(0, 117, 53, 1)',
        pointBoarderColor: 'rgba(0, 117, 53,, 1)',
        backgroundColor: 'rgba(0, 117, 53,, 0.4)',
        pointHoverBackgroundColor: 'rgba(0, 117, 53,, 1)',
        pointHoverBorderColor: 'rgba(0, 117, 53,, 1)',
        spanGaps: true,
      }           
    ]
  };  


  const temperatureChartOptions = {
    scales: {
      yAxes: [{
        id: 'Temperature',
        type: 'linear',
        scaleLabel: {
          labelString: 'Temperature [ºC]',
          display: true,
        },
        position: 'left',
      },
      {
        id: 'Humidity',
        type: 'linear',
        scaleLabel: {
          labelString: 'Humidity [%]',
          display: true,
        },
        position: 'right',
      }]
    }
  };  

  const pressureChartOptions = {
    scales: {
      yAxes: [
      {
        id: 'Pressure',
        type: 'linear',
        scaleLabel: {
          labelString: 'Pressure [kPa]',
          display: true,
        },
        position: 'left',
      }]
    }
  };

  const humidityChartOptions = {
    scales: {
      yAxes: [
      {
        id: 'Humidity',
        type: 'linear',
        scaleLabel: {
          labelString: 'Humidity [%]',
          display: true,
        },
        position: 'left',
      }]
    }
  };

  const accelerationChartOptions = {
    scales: {
      yAxes: [
      {
        id: 'Acceleration',
        type: 'linear',
        scaleLabel: {
          labelString: 'Acceleration [mg]',
          display: true,
        },
        position: 'left',
      }]
    }
  };

  // Get the context of the canvas element we want to select
  const ctx = document.getElementById('temperatureChart').getContext('2d');
  const temperatureChart = new Chart(
    ctx,
    {
      type: 'line',
      data: temperatureChartData,
      options: temperatureChartOptions,
    });

  const ctx2 = document.getElementById('pressureChart').getContext('2d');
  const pressureChart = new Chart(
    ctx2,
    {
      type: 'line',
      data: pressureChartData,
      options: pressureChartOptions,
    });

  // Get the context of the canvas element we want to select
  const ctx3 = document.getElementById('humidityChart').getContext('2d');
  const humidityChart = new Chart(
    ctx3,
    {
      type: 'line',
      data: humidityChartData,
      options: humidityChartOptions,
    });

  const ctx4 = document.getElementById('accelerationChart').getContext('2d');
  const accelerationChart = new Chart(
    ctx4,
    {
      type: 'line',
      data: accelerationChartData,
      options: accelerationChartOptions,
    });


  // Manage a list of devices in the UI, and update which device data the chart is showing
  // based on selection
  let needsAutoSelect = true;
  const deviceCount = document.getElementById('deviceCount');
  const listOfDevices = document.getElementById('listOfDevices');
  function OnSelectionChange() {
    const device = trackedDevices.findDevice(listOfDevices[listOfDevices.selectedIndex].text);
    temperatureChartData.labels = device.timeData;
    temperatureChartData.datasets[0].data = device.temperatureData;
    //temperatureChartData.datasets[1].data = device.humidityData;
    temperatureChart.update();
    pressureChartData.labels = device.timeData;
    pressureChartData.datasets[0].data = device.pressureData;
    pressureChart.update();    
    humidityChartData.labels = device.timeData;
    humidityChartData.datasets[0].data = device.humidityData;
    humidityChart.update();  
    accelerationChartData.labels = device.timeData;
    accelerationChartData.datasets[0].data = device.accelerationXData;
    accelerationChartData.datasets[1].data = device.accelerationYData;
    accelerationChartData.datasets[2].data = device.accelerationZData;
    accelerationChart.update();          
  }
  listOfDevices.addEventListener('change', OnSelectionChange, false);

  // When a web socket message arrives:
  // 1. Unpack it
  // 2. Validate it has date/time and temperature
  // 3. Find or create a cached device to hold the telemetry data
  // 4. Append the telemetry data
  // 5. Update the chart UI
  webSocket.onmessage = function onMessage(message) {
    try {
      const messageData = JSON.parse(message.data);
      console.log(messageData);

      // time and either temperature or pressure are required
      if (!messageData.MessageDate || (!messageData.IotData["TIDS_T[°C]"] && !messageData.IotData["PADS_P[kPa]"] && !messageData.IotData["HIDS_RH[%]"] && !messageData.IotData["ITDS_X[mg]"] && !messageData.IotData["ITDS_Y[mg]"] && !messageData.IotData["ITDS_Z[mg]"])) {
        return;
      }

      // find or add device to list of tracked devices
      const existingDeviceData = trackedDevices.findDevice(messageData.DeviceId);

      if (existingDeviceData) {
        existingDeviceData.addData(messageData.MessageDate, messageData.IotData["TIDS_T[°C]"], messageData.IotData["PADS_P[kPa]"], messageData.IotData["PADS_T[°C]"], messageData.IotData["HIDS_RH[%]"], messageData.IotData["HIDS_T[°C]"], messageData.IotData["ITDS_X[mg]"], messageData.IotData["ITDS_Y[mg]"], messageData.IotData["ITDS_Z[mg]"]);
      } else {
        const newDeviceData = new DeviceData(messageData.DeviceId);
        trackedDevices.devices.push(newDeviceData);
        const numDevices = trackedDevices.getDevicesCount();
        deviceCount.innerText = numDevices === 1 ? `${numDevices} device` : `${numDevices} devices`;
        newDeviceData.addData(messageData.MessageDate, messageData.IotData["TIDS_T[°C]"], messageData.IotData["PADS_P[kPa]"], messageData.IotData["PADS_T[°C]"], messageData.IotData["HIDS_RH[%]"], messageData.IotData["HIDS_T[°C]"], messageData.IotData["ITDS_X[mg]"], messageData.IotData["ITDS_Y[mg]"], messageData.IotData["ITDS_Z[mg]"]);

        // add device to the UI list
        const node = document.createElement('option');
        const nodeText = document.createTextNode(messageData.DeviceId);
        node.appendChild(nodeText);
        listOfDevices.appendChild(node);

        // if this is the first device being discovered, auto-select it
        if (needsAutoSelect) {
          needsAutoSelect = false;
          listOfDevices.selectedIndex = 0;
          OnSelectionChange();
        }
      }

      temperatureChart.update();
      pressureChart.update();
      humidityChart.update();
      accelerationChart.update();

    } catch (err) {
      console.error(err);
    }
  };
});
