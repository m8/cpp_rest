import http from 'k6/http';
import { check, sleep } from 'k6';

export let options = {
    duration: '15s', 
    vus: 10,
};

const BASE_URL = 'http://localhost:18080';

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

    //sleep(0.1);
}
