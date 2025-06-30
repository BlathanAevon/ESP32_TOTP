window.onload = () => {
  getServices();
  getIsSynced();
};

async function openModal(networkName) {
  let modal = document.getElementById("connectionModal");
  modal.style.display = "flex";
  document.getElementById("networkModalName").value = networkName;
}

async function closeModal() {
  let modal = document.getElementById("connectionModal");
  document.getElementById("connectionStatusMsg").textContent = "";
  modal.style.display = "none";
}

async function addService(serviceName = "", serviceSecret = "") {
  const container = document.getElementById("service-container");
  const newRow = document.createElement("div");
  newRow.className = "form-row";
  newRow.innerHTML = `
        <div class="form-group">
          <input type="text" class="serviceInput" name="service[]" required autocomplete="off" aria-required="true" placeholder="Service Name" value=${serviceName}>
        </div>
        <div class="form-group">
          <input type="password" class="secretInput" name="secret[]" required autocomplete="new-password" aria-required="true" placeholder="Secret key" value=${serviceSecret}>
        </div>
        <button type="button" class="action-btn remove" onclick="removeService(this)" title="Remove this service entry">-</button>
      `;
  container.appendChild(newRow);
}

async function removeService(btn) {
  if (document.querySelectorAll(".form-row").length > 0) {
    btn.parentElement.remove();
  } else {
    alert("At least one service entry is required.");
  }
}

async function getServices() {
  try {
    const response = await fetch("/getServices");

    if (!response.ok) {
      throw new Error(`Response status: ${response.status}`);
    }

    let jsonData = JSON.parse(await response.text());

    for (let elem of jsonData) {
      await addService(elem.name, elem.secret);
    }
  } catch (error) {
    console.error(error.message);
  }
}

async function getIsSynced() {
  const rtcStatus = document.getElementById("rtc-status-container");
  const response = await fetch("/isRTCsynced");

  if ((await response.text()) == "SYNCED") {
    rtcStatus.innerHTML = `
            <span>RTC Synced</span>
            <div class="network-indicator-container rounded full">
            </div>
        `;
  }
}

async function saveServices() {
  try {
    let servicesArray = [];

    const services = document.getElementsByClassName("serviceInput");
    const secrets = document.getElementsByClassName("secretInput");

    for (let i = 0; i <= services.length - 1; i++) {
      if (services.length > 0 && secrets.length > 0) {
        let service = services.item(i).value;
        let secret = secrets.item(i).value;

        if (service && secret) {
          let s = {};
          s.name = service;
          s.secret = secret;
          servicesArray.push(s);
        }
      }
    }

    if (servicesArray.length > 0 && secrets.length > 0) {
      const servicesJson = JSON.stringify(servicesArray);
      const response = await fetch("/saveServices", {
        method: "POST",
        body: servicesJson,
        headers: {
          "Content-type": "application/json; charset=UTF-8",
        },
      });

      if (!response.ok) {
        throw new Error(`Response status: ${response.status}`);
      }
      return;
    }
  } catch (error) {
    console.log(error.message);
  }
}

async function scanForNetworks() {
  let tries = 0;
  const networksContainer = document.getElementById("networks-container");
  networksContainer.innerHTML = "";
  document.getElementById("scanButton").innerHTML = "Scanning...";

  try {
    const response = await fetch("/startWifiScan");

    if (!response.ok) {
      throw new Error(`Response status: ${response.status}`);
    }

    const interval = setInterval(async () => {
      let scanResponse = await fetch("/getWifiScan");

      if (!scanResponse.ok) {
        throw new Error(`Response status: ${scanResponse.status}`);
      }

      let responseText = await scanResponse.text();

      if (responseText != "scan not finished") {
        let jsonData = JSON.parse(responseText);

        networksContainer.style = "display: block";

        for (let elem of jsonData) {
          const connectionButton = document.createElement("button");
          connectionButton.type = "button";
          connectionButton.className = "wifiButton";
          connectionButton.setAttribute("onClick", `openModal("${elem.SSID}")`);
          connectionButton.textContent = `${elem.SSID} ${elem.RSSI}`;
          networksContainer.append(connectionButton);
        }

        clearInterval(interval);
        document.getElementById("scanButton").innerHTML = "Scan for networks";
        return;
      }

      if (tries >= 5) {
        clearInterval(interval);
        document.getElementById("scanButton").innerHTML = "Scan for networks";
        return;
      }

      tries++;
    }, 3000);
  } catch (error) {
    console.log(error.message);
  }
}

async function connectToNetwork(ssid, pass) {
  let connectionTries = 0;

  if (ssid == "") {
    alert("SSID IS EMPTY!");
    return;
  }

  if (pass == "") {
    alert("PASSWORD IS EMPTY!");
    return;
  }

  document.getElementById("wifiConnectButton").innerHTML = "Connecting...";
  try {
    const response = await fetch("/connectToNetwork", {
      method: "POST",
      body: JSON.stringify({
        ssid: ssid,
        psk: pass,
      }),
      headers: {
        "Content-type": "application/json; charset=UTF-8",
      },
    });

    if (!response.ok) {
      throw new Error(`Response status: ${response.status}`);
    }

    document.getElementById("wifiConnectButton").innerHTML = "Connecting...";

    const interval = setInterval(async () => {
      const isConnected = await fetch("/isConnected");

      if (!isConnected.ok) {
        throw new Error(`Response status: ${isConnected.status}`);
      }

      const jsonData = JSON.parse(await isConnected.text());

      if (jsonData.success) {
        const saveSettingsResponse = await fetch("/saveWifiSettings");
        const syncRTCResponse = await fetch("/syncRTC");

        if (!saveSettingsResponse.ok || !syncRTCResponse.ok) {
          throw new Error(
            `Response status: ${saveSettingsResponse.status} ${syncRTCResponse.status}`
          );
        }

        const disconnectResponse = await fetch("/disconnectFromNetwork");

        if (!disconnectResponse.ok) {
          throw new Error(`Response status: ${disconnectResponse.status}`);
        }

        document.getElementById("wifiConnectButton").innerHTML = "Connect";

        const statusMsg = document.getElementById("connectionStatusMsg");
        statusMsg.style.color = "green";
        statusMsg.textContent = "Connected";

        clearInterval(interval);
        getIsSynced();
        closeModal();
        return;
      }

      connectionTries++;
      if (connectionTries >= 10) {
        document.getElementById("wifiConnectButton").innerHTML = "Connect";

        const statusMsg = document.getElementById("connectionStatusMsg");
        statusMsg.style.color = "red";
        statusMsg.textContent = "Failed to connect";

        clearInterval(interval);
        return;
      }
      connectionTries++;
    }, 1000);
  } catch (error) {
    console.log(error.message);
  }
}
