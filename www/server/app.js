/**
 * GP2040 Configurator Development Server
 */

const express = require('express');
const cors = require('cors');

const port = process.env.PORT || 8080;
const app = express();
app.use(cors());
app.use(express.json());

app.get('/api/getTestData', (req, res) => {
	console.log('/api/getTestData');
	return res.send({
		testString: 'test1',
		testNumber: 1.35,
		testArray: [
			"child1",
			"child2",
			"child3",
		],
	});
});

app.get('/api/setTestData', (req, res) => {
	console.log('/api/setTestData');
	return res.send(req.data);
});

app.post('/api/*', (req, res) => {
	console.log(req.url);
	return res.send(req.body);
})

app.listen(port, () => {
  console.log(`Example app listening at http://localhost:${port}`)
});
