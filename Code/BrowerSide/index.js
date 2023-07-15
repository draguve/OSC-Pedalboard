var pedal_port = null;

window.addEventListener("DOMContentLoaded", (event) => {
  const notSupported = document.getElementById("NotSupported");
  const connectButton = document.getElementById("ConnectForm");
  var isSerialSupported = "serial" in navigator;
  if(isSerialSupported){
    notSupported.style.display = 'none';
  }else{
    connectButton.style.display = 'none';
  }
});

async function connectToPedal(){
    try {
        const port = await navigator.serial.requestPort();
        console.log(port);
        pedal_port = port;
        M.toast({html: 'Connected To Pedal'});
      } catch (error) {
        M.toast({html: 'Could not connect to any device'});
      }
}

async function getLatestValues(){
    
}