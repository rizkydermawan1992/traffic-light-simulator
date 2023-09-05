function startConnect(){
    host = document.getElementById("host").value;
    port = document.getElementById("port").value;
    clientID = "clientID" + parseInt(Math.random()*100);

    btn_connect = document.getElementById("btn-connect");
    btn_connect.value = "Connecting . . .";
    btn_connect.style.backgroundColor = "gray";
    console.log("connecting to "+host+" with port "+ port);
    client = new Paho.MQTT.Client(host, Number(port), clientID); 

    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;

    client.connect({
        onSuccess : onConnect,
        onFailure : onFailure
    });
}

function onConnect(){
    btn_connect = document.getElementById("btn-connect");
    topic_s = document.getElementById("topic_s").value;
    client.subscribe(topic_s);

    btn_connect.value = "Disconnect";
    btn_connect.style.backgroundColor = "red";
}

function onFailure(err){
    console.log("Connection Failed! Error message: "+err.errorMessage);
}


function onConnectionLost(response){
    console.log(" Connection Lost!!!");
    if(response != 0){
        console.log("Error Message: "+ response.errorMessage)
    }
}

function onMessageArrived(message){
    console.log("Message Arrived: "+message.payloadString);
    red = document.getElementById("red");
    yellow = document.getElementById("yellow");
    green = document.getElementById("green");
    
    const colorSettings = {
        "111": ["red", "yellow", "green"],
        "001": ["grey", "grey", "green"],
        "011": ["grey", "yellow", "green"],
        "010": ["grey", "yellow", "grey"],
        "100": ["red", "grey", "grey"],
        default: ["grey", "grey", "grey"]
    };
    const colors = colorSettings[message.payloadString] || colorSettings.default;
    red.style.backgroundColor = colors[0];
    yellow.style.backgroundColor = colors[1];
    green.style.backgroundColor = colors[2];

}

function startDisconnect(){
  document.getElementById("red").style.backgroundColor = "red";
  document.getElementById("yellow").style.backgroundColor = "yellow";
  document.getElementById("green").style.backgroundColor = "green";

  document.getElementById("btn-connect").value = "Connect"
  document.getElementById("btn-connect").style.backgroundColor = "#2980b9";
  client.disconnect();
}

function publishMessage(callback) {
    topic_p = document.getElementById("topic_p").value;
    red_delay  = document.getElementById("red_delay").value;
    green_delay = document.getElementById("green_delay").value;

    jsonData = {
        red_delay : red_delay,
        green_delay : green_delay
    };

    msg = JSON.stringify(jsonData);

    Message = new Paho.MQTT.Message(msg);
    Message.destinationName = topic_p;

    try {
        client.send(Message);
        console.log("Publish Message: " + Message);
        callback(true); // Pesan berhasil dikirim
    } catch (error) {
        console.error("Error while publishing message: " + error);
        callback(false); // Pesan gagal dikirim
    }
}



