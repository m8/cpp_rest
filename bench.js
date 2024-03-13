import http from 'k6/http';
import { check, sleep } from 'k6';

export let options = {
    stages: [
        { duration: '15s', target: 100 }, 
        { duration: '2s', target: 30 },   
        { duration: '5s', target: 0 },   
    ],
};

const BASE_URL = 'http://localhost:18081';

let initialTeamCount = 100;
const getTeamsResponse = () => http.get(`${BASE_URL}/teams`);
const postTeamsResponse = () => http.post(`${BASE_URL}/teams`);
const getTeamIdResponse = (id) => http.get(`${BASE_URL}/team/${id}`);
const deleteTeamIdResponse = (id) => http.del(`${BASE_URL}/team/${id}`);
const patchTeamIdResponse = (id) => http.patch(`${BASE_URL}/team/${id}`);

export default function () {
    
    let getTeamsRes = getTeamsResponse();
    check(getTeamsRes, {
        'get teams status is 200': (r) => r.status === 200,
    });

    sleep(0.1);
}
