const fs = require('fs');
const path = require('path');
const axios = require('axios');

const API_KEY = 'a6e19d064ee244afb50f6f9d647f3ccb';
const layer = 'cycle';

// Hier der Bereich als Lat/Lon
const latMin = 53.833613;  // Südliche Grenze
const latMax = 53.836177;  // Nördliche Grenze
const lonMin = 10.697529;  // Westliche Grenze
const lonMax = 10.702207;  // Östliche Grenze

const zoomMin = 17;
const zoomMax = 21;

function long2tileX(lon, zoom) {
    return Math.floor((lon + 180) / 360 * Math.pow(2, zoom));
}

function lat2tileY(lat, zoom) {
    const latRad = lat * Math.PI / 180;
    return Math.floor(
        (1 - Math.log(Math.tan(latRad) + 1 / Math.cos(latRad)) / Math.PI) / 2 * Math.pow(2, zoom)
    );
}

async function downloadTile(z, x, y) {
    const url = `https://tile.thunderforest.com/${layer}/${z}/${x}/${y}.png?apikey=${API_KEY}`;
    const folderPath = path.join(__dirname, 'tiles', layer, String(z), String(x));
    const filePath = path.join(folderPath, `${y}.png`);

    if (!fs.existsSync(folderPath)) {
        fs.mkdirSync(folderPath, { recursive: true });
    }

    try {
        const response = await axios.get(url, { responseType: 'stream' });
        const writer = fs.createWriteStream(filePath);
        response.data.pipe(writer);

        return new Promise((resolve, reject) => {
            writer.on('finish', () => {
                console.log(`✔️  Tile ${z}/${x}/${y} gespeichert.`);
                resolve();
            });
            writer.on('error', (err) => {
                console.error(`❌ Fehler beim Speichern von Tile ${z}/${x}/${y}:`, err.message);
                reject(err);
            });
        });
    } catch (error) {
        console.error(`⚠️ Fehler beim Download von Tile ${z}/${x}/${y}:`, error.message);
    }
}

async function downloadTiles() {
    for (let z = zoomMin; z <= zoomMax; z++) {
        const xMin = long2tileX(lonMin, z);
        const xMax = long2tileX(lonMax, z);
        const yMin = lat2tileY(latMax, z); // Achtung: y-Koordinate invertiert bei lat!
        const yMax = lat2tileY(latMin, z);

        for (let x = xMin; x <= xMax; x++) {
            for (let y = yMin; y <= yMax; y++) {
                await downloadTile(z, x, y);
            }
        }
    }
}

downloadTiles();
