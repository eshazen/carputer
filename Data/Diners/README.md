
With considerable help from ChatGPT:

## How to run & download CSV (Overpass Turbo web UI)
* Open: https://overpass-turbo.eu/
* In the editor, replace the default text with the query above.
* Click Run (the lightning bolt). The matching objects will be fetched and shown on the map.
* After it finishes, click the Export button → choose download/copy as CSV (Overpass Turbo will generate the CSV using the out:csv specification). Save the file to your computer.
* If Overpass Turbo displays a size/timeout warning, reduce the radius or run the query in smaller geographic tiles (I can provide a tiled query script if needed).

```
[out:csv(::id, type, name, amenity, cuisine, "addr:street", "addr:housenumber", "addr:city", ::lat, ::lon; true; ",")][timeout:180];
(
  node["name"~"[Dd]iner"](around:48280,42.332137,-71.121679);
  way["name"~"[Dd]iner"](around:48280,42.332137,-71.121679);
  relation["name"~"[Dd]iner"](around:48280,42.332137,-71.121679);
);
out center;
```

