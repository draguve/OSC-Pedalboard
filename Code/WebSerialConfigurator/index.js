var pedal_port = null;
var textEncoder = null;
var writableStreamClosed = null;
var writer = null;

const encoder = new TextEncoder();
const decoder = new TextDecoder();
this.messages = [];

function delay(time) {
  return new Promise(resolve => setTimeout(resolve, time));
}

window.addEventListener("DOMContentLoaded", (event) => {
  const notSupported = document.getElementById("NotSupported");
  const connectButton = document.getElementById("ConnectForm");
  var isSerialSupported = "serial" in navigator;
  if (isSerialSupported) {
    notSupported.style.display = "none";
  } else {
    connectButton.style.display = "none";
  }
});

async function sendData(pedal_port,input_data){
  if (pedal_port.writable) {
    const writer = pedal_port.writable.getWriter();
    await writer.write(encoder.encode(input_data + "\r\n"));
    writer.releaseLock();
  }
}

async function sendDataGetOutput(pedal_port,input_data){
  this.messages = [];
  await sendData(pedal_port,input_data);
  await delay(500);
  try{
    return JSON.parse(this.messages.join(""));
  }catch{
    return {"parseFailed":true};
  }
}

async function monitor() {
  console.log('monitor()');
  while (pedal_port && pedal_port?.readable) {
    const reader = pedal_port.readable.getReader();
    this.open = true;
    try {
      while (this.open) {
        console.log('reading...');
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
      console.error('reading error', error);
    } finally {
      reader.releaseLock();
    }
  }
}

async function GetSettingsFromDevice(){
  var settings = await sendDataGetOutput(pedal_port,'{"getSettings":1}');
  console.log(settings);
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
    var settings = await sendDataGetOutput(pedal_port,'{"getSettings":1}');
    console.log(settings);
    M.toast({ html: "Connected To Pedal" });
  } catch (error) {
    console.log(error);
    M.toast({ html: "Could not connect to any device" });
  }
}

async function getSettings() {}
