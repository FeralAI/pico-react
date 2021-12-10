import React, { useEffect, useState } from 'react';
import logo from './logo.png';
import './App.scss';

// Env vars from the .env files must be prefixed with "REACT_APP_"!
const currentVersion = process.env.REACT_APP_CURRENT_VERSION;
const baseUrl = process.env.NODE_ENV === 'production' ? '' : 'http://localhost:8080';

function App() {
	const [values, setValues] = useState({});
	
	useEffect(() => {
		fetch(`${baseUrl}/api/getTestData`)
			.then(repsonse => repsonse.json())
			.then(data => setValues(data));
	}, [setValues]);

	const updateValues = () => {
		const data = { "testString": "test2", "testNumber": 2.70, "testArray": ["child4", "child5", "child6"] };
		fetch(`${baseUrl}/api/setTestData`, {
			method: 'POST',
			mode: 'cors',
			headers: { 'Content-Type': 'application/json' },
			body: JSON.stringify(data),
		})
			.then(repsonse => repsonse.json())
			.then(result => setValues(result));

		console.log('updated');
	};

	return (
		<div className="App">
			<header className="App-header">
				<img src={logo} className="App-logo" alt="logo" />
				<div>Current Version: {currentVersion}</div>
				<div>Current Values: {JSON.stringify(values)}</div>
				<button onClick={updateValues}>Test</button>
			</header>
		</div>
	);
}

export default App;
