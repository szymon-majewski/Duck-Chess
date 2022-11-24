const express = require('express');
const app = express();
const port = 3000;

app.use(express.static('../client/html'));
app.use('/css', express.static("../client/css"));
app.use('/js', express.static("../client/js"));
app.use('/res', express.static("../client/res"));

app.use(express.json({limit: '1mb'}));

app.listen(port, () => console.log('listening on port ' + port));

app.post('/api', (request, response) => 
{
    console.log(request.body);
});