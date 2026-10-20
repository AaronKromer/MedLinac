import argparse
import xml.etree.ElementTree as ET
import csv

def normalize_bins(data):
    bins = {int(bin1d.get("binNum")): bin1d
            for bin1d in data.findall("./bin1d")}
    for bin_num in range(120):
        if bin_num not in bins:
            bins[bin_num] = ET.Element("bin1d", {
                "binNum": str(bin_num),
                "entries": "0",
                "error": "0.000000e+00",
                "height": "0.000000e+00",
                "weightedMean": "0.000000e+00",
            })

    data[:] = [bins[bin_num] for bin_num in sorted(bins)]
    return data.findall("./bin1d")
 

parser = argparse.ArgumentParser()
parser.add_argument("--file", "-f", type=str, required=True, help="File to transform")
parser.add_argument("--outputfilename", "-o", type=str, help="Output file name", default="MedlinacDiane.csv")
args=parser.parse_args()

tree1 = ET.parse(args.file)
root1 = tree1.getroot()
names=[]
for child in root1:
    keys=child.attrib.keys()
    if "name" in child.attrib.keys():
        names.append(child.attrib["name"])

with open(args.outputfilename, "w", newline="", encoding="utf-8") as csvfile:
    # ";" is the default Excel list separator on Italian Windows locales
    writer = csv.writer(csvfile, delimiter=",")
    writer.writerow(["name", "title", "binNum", "weightedMean", "height", "error", "entries"])

    for name in names:
        histogram = root1.find(f"./histogram1d[@name='{name}']")
        title = histogram.get("title")

        data1 = histogram.find("./data1d")
        graphs = normalize_bins(data1)

        for bin1d in graphs:
            writer.writerow([
                name,
                title,
                bin1d.get("binNum"),
                # fixed-point notation instead of scientific, easier to read/plot
                f"{float(bin1d.get('weightedMean')):.4f}",
                f"{float(bin1d.get('height')):.6f}",
                f"{float(bin1d.get('error')):.6f}",
                bin1d.get("entries"),
            ])

print(f"Wrote {len(names)} histograms to {args.outputfilename}")




