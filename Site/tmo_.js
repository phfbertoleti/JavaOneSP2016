(function() {
	window.Main = {};
	Main.Page = (function() {
		var mosq = null;
		var topic = $('#sub-topic-text').val();
		function Page() {
			var _this = this;
			mosq = new Mosquitto();

			$('#connect-button').click(function() {
				return _this.connect();
			});
			$('#disconnect-button').click(function() {
				return _this.disconnect();
			});
			$('#subscribe-button').click(function() {
				return _this.subscribe();
			});
			$('#unsubscribe-button').click(function() {
				return _this.unsubscribe();
			});
			mosq.onconnect = function(rc){
				var p = document.createElement("p");
				var topic = localStorage.getItem("topic");
				p.innerHTML = "Conectado ao servidor!";
				$("#debug").append(p);
				mosq.subscribe(topic, 0);
			};
			mosq.ondisconnect = function(rc){
				var p = document.createElement("p");
				var url = "ws://iot.eclipse.org/ws";
				
				p.innerHTML = "Desconectado do servidor.";
				$("#debug").append(p);
				$("#status_io").html("");
			};
			mosq.onmessage = function(topic, payload, qos){
				var p = document.createElement("p");
				var payload_parseado = payload.split(">");
				
				p.innerHTML = "<table><td width='50%'><tr>Modulo 1:<br> Sa?da 1: <img src='"+payload_parseado[0]+"'.png></tr><tr>Sa?da 2: <img src='"+payload_parseado[1]+"'.png></tr><tr>Sa?da 3: <img src='"+payload_parseado[2]+"'.png></tr><tr>Sa?da 4: <img src='"+payload_parseado[3]+"'.png></tr></td><td width='50%'><tr>Modulo 2:<br> Sa?da 1: <img src='"+payload_parseado[4]+"'.png></tr><tr>Sa?da 2: <img src='"+payload_parseado[5]+"'.png></tr><tr>Sa?da 3: <img src='"+payload_parseado[6]+"'.png></tr><tr>Sa?da 4: <img src='"+payload_parseado[7]+"'.png></tr></td></table>";								
				$("#status_io").html(p);
				
				}
			};
		}
		Page.prototype.connect = function(){
			var url = "ws://iot.eclipse.org/ws";
			mosq.connect(url);
		};
		Page.prototype.disconnect = function(){
			mosq.disconnect();
		};
		Page.prototype.subscribe = function(){
			mosq.subscribe(topic, 0);
		};
		Page.prototype.unsubscribe = function(){
			mosq.unsubscribe(topic);
		};

		return Page;
	})();
	$(function(){
		return Main.controller = new Main.Page;
	});
}).call(this);
