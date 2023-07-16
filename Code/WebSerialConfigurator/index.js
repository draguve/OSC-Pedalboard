var pedal_port = null;
var textEncoder = null;
var writableStreamClosed = null;
var writer = null;

const encoder = new TextEncoder();
const decoder = new TextDecoder();
this.messages = [];
var clicked = 0;

$(".toggle-password").click(function (e) {
  e.preventDefault();

  $(this).toggleClass("toggle-password");
  if (clicked == 0) {
    $(this).html('<span class="material-icons">visibility_off</span >');
    clicked = 1;
  } else {
    $(this).html('<span class="material-icons">visibility</span >');
    clicked = 0;
  }

  var input = $($(this).attr("toggle"));
  if (input.attr("type") == "password") {
    input.attr("type", "text");
  } else {
    input.attr("type", "password");
  }
});

const onconnect = (e) => {
  console.log(id + "device connected", e);
  this.port = e.target;
  this.physicallyConnected = true;
};
navigator.serial.addEventListener("connect", onconnect);

// notification for a USB device getting physically disconnected
const ondisconnect = (e) => {
  console.log(id + " disconnect");
  this.physicallyConnected = false;
  this.open = false;
};
navigator.serial.addEventListener("disconnect", ondisconnect);

function delay(time) {
  return new Promise((resolve) => setTimeout(resolve, time));
}

window.addEventListener("DOMContentLoaded", (event) => {
  $("#ConnectForm").hide();
  const notSupported = document.getElementById("NotSupported");
  const connectButton = document.getElementById("ConnectForm");
  var isSerialSupported = "serial" in navigator;
  if (isSerialSupported) {
    notSupported.style.display = "none";
  } else {
    connectButton.style.display = "none";
  }
});

async function sendData(pedal_port, input_data) {
  if (pedal_port.writable) {
    const writer = pedal_port.writable.getWriter();
    await writer.write(encoder.encode(input_data + "\r\n"));
    writer.releaseLock();
  }
}

async function sendDataGetOutput(pedal_port, input_data) {
  this.messages = [];
  await sendData(pedal_port, input_data);
  await delay(500);
  try {
    return JSON.parse(this.messages.join(""));
  } catch {
    return { parseFailed: true };
  }
}

async function monitor() {
  console.log("monitor()");
  while (pedal_port && pedal_port?.readable) {
    const reader = pedal_port.readable.getReader();
    this.open = true;
    try {
      while (this.open) {
        const { value, done } = await reader.read();
        if (done) {
          // |reader| has been canceled.
          this.open = false;
          break;
        }
        const decoded = decoder.decode(value);
        //console.log('read complete:', decoded, value, done);
        this.messages.push(decoded);
      }
    } catch (error) {
      console.error("reading error", error);
    } finally {
      reader.releaseLock();
    }
  }
}

async function GetSettingsFromDevice() {
  var settings = await sendDataGetOutput(pedal_port, '{"getSettings":1}');
  UpdateSettingsFromDevice(settings);
  M.toast({ html: "Updated Settings" });
}

async function GetWifiStatusFromDevice() {
  var wifiStatus = await sendDataGetOutput(pedal_port, '{"getWifiStatus":1}');
  UpdateWifiStatusFromDevice(wifiStatus);
  M.toast({ html: "Updated Wifi Status" });
}

async function UpdateSettingsFromDevice(settings) {
  $("#Wifi_SSID").val(settings.ssid);
  $("#password").val(settings.password);
  $("#IPAddress").val(settings.OSC_Server);
  $("#Port").val(settings.OSC_Port);
  M.updateTextFields();
}

async function UpdateWifiStatusFromDevice(wifiStatus) {
  $("#connectedToWifi").prop("checked", wifiStatus.is_connected);
  $("#AssignedIPAddress").val(wifiStatus.ifconfig[0]);
  $("#SubnetMask").val(wifiStatus.ifconfig[1]);
  $("#Gateway").val(wifiStatus.ifconfig[2]);
  $("#DNS").val(wifiStatus.ifconfig[3]);
  M.updateTextFields();
}

async function PushSettingsToDevice() {
  port = parseInt($("#Port").val()) || 8000;
  ip = $("#IPAddress").val();
  if (!ValidateIPaddress(ip)) {
    M.toast({ html: "Could not push, Invalid IP" });
    return;
  }
  data = {
    ssid: $("#Wifi_SSID").val(),
    OSC_Server: ip,
    password: $("#password").val(),
    OSC_Port: port,
  };
  settingsToPush = JSON.stringify(data);
  await sendData(pedal_port, settingsToPush);
  M.toast({ html: "Settings pushed to device" });
}

function ValidateIPaddress(ipaddress) {
  if (
    /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(
      ipaddress
    )
  ) {
    return true;
  }
  return false;
}

async function connectToPedal() {
  try {
    pedal_port = await navigator.serial.requestPort();
    await pedal_port.open({
      baudRate: 115200,
      dataBits: 8,
      stopBits: 1,
      parity: "none",
      flowControl: "none",
    });
    monitor();
    var settings = await sendDataGetOutput(pedal_port, '{"getSettings":1}');
    var wifiStatus = await sendDataGetOutput(pedal_port, '{"getWifiStatus":1}');
    UpdateWifiStatusFromDevice(wifiStatus);
    if (!("password" in settings)) {
      M.toast({ html: "Could not connect to any device" });
      return;
    }
    UpdateSettingsFromDevice(settings);
    $("#ConnectForm").show();
    $("#ConnectButton").hide();
    M.toast({ html: "Connected To Pedal" });
  } catch (error) {
    console.log(error);
    M.toast({ html: "Could not connect to any device" });
  }
}

async function getSettings() {}
