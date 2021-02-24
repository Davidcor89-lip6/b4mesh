importScripts('../configuration.js')

var myRequest = new Request(configuration.blockgraph_as_dot_API_accesspoint);
setInterval(() => {
    fetch(myRequest)
        .then(function(reponse) {
            return reponse.text();
        })
        .then(data => {
            //cache = data;
            self.postMessage(data);
        })
}, configuration.refresh_rate_ms);
