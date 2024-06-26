import http from 'k6/http';
import { check, sleep } from 'k6';

export let options = {
    duration: '15s', 
    vus: 10,
};

const BASE_URL = 'http://localhost:18080';

const getTeamsResponse = () => http.get(`${BASE_URL}/teams`);
const postTeamsResponse = () => http.post(`${BASE_URL}/teams`);
const getTeamIdResponse = (id) => http.get(`${BASE_URL}/team/${id}`);
const deleteTeamIdResponse = (id) => http.del(`${BASE_URL}/team/${id}`);
const patchTeamIdResponse = (id) => http.patch(`${BASE_URL}/team/${id}`);

function getRandomInt(min, max) {
  min = Math.ceil(min);
  max = Math.floor(max);
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

export default function () {
    let random_team_id = getRandomInt(0, 20000);
    let getTeamsRes = getTeamIdResponse(random_team_id);
    check(getTeamsRes, {
        'get teams status is 200': (r) => r.status === 200,
    });
}
