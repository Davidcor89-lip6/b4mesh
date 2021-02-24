const configuration = {
    blockgraph_as_dot_API_accesspoint : 'http://127.0.0.1:8000/tests/datas/blockgraph.dot.json',
    refresh_rate_ms : 3000
};

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
