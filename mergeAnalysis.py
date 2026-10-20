import argparse
import xml.etree.ElementTree as ET

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
parser.add_argument("--file1", "-f1", type=str, help="File 1 to merge")
parser.add_argument("--file2", "-f2", type=str, help="File 2 to merge")
parser.add_argument("--outputfilename", "-o", type=str, help="Output file name", default="MergedMedicalinac.xml")
args=parser.parse_args()

tree1 = ET.parse(args.file1)
#tree1 = ET.parse("C:/Users/akrom/Downloads/MedLinacDiane800M.xml")
root1 = tree1.getroot()
tree2 = ET.parse(args.file2)
#tree2 = ET.parse("C:/Users/akrom/Downloads/MedLinacDiane700M.xml")
root2 = tree2.getroot()
names=[]
for child in root1:
    keys=child.attrib.keys()
    if "name" in child.attrib.keys():
        names.append(child.attrib["name"])

for name in names:
    histogram = root1.find(f"./histogram1d[@name='{name}']")
    histogram2 = root2.find(f"./histogram1d[@name='{name}']")

    statistics1 = histogram.find("./statistics")
    statistics2 = histogram2.find("./statistics")
    entries1=int(statistics1.get('entries'))
    entries2=int(statistics2.get('entries'))
    entriesSum = entries1 + entries2
    statistics1.set("entries", str(entriesSum))

    for statistic in statistics1.findall("./statistic"):
        direction = statistic.get("direction")
        statistic2 = statistics2.find(f"./statistic[@direction='{direction}']")
        mean1 = float(statistic.get("mean"))
        mean2 = float(statistic2.get("mean"))
        rms1 = float(statistic.get("rms"))
        rms2 = float(statistic2.get("rms"))
        meanSum = (entries1 * mean1 + entries2 * mean2) / entriesSum
        rmsSum = ((entries1 * rms1**2 + entries2 * rms2**2) / entriesSum) ** 0.5
        statistic.set("mean", f"{meanSum:.6e}")
        statistic.set("rms", f"{rmsSum:.6e}")

    data1 = histogram.find("./data1d")
    data2 = histogram2.find("./data1d")

    graphs = normalize_bins(data1)
    graphs2 = normalize_bins(data2)


    for i in range(len(graphs)):
        height1=graphs[i].get('height')
        height2=graphs2[i].get('height')
        heightsSum = float(height1) + float(height2)
        graphs[i].set("height", f"{heightsSum:.6e}")

        entries1=int(graphs[i].get('entries'))
        entries2=int(graphs2[i].get('entries'))
        entriesSum = entries1 + entries2
        graphs[i].set("entries", str(entriesSum))

        weightedMean1=graphs[i].get('weightedMean')
        weightedMean2=graphs2[i].get('weightedMean')
        if entriesSum:
            weightedMeanSum = (float(weightedMean1)*entries1 + float(weightedMean2)*entries2)/entriesSum
        else:
            weightedMeanSum = 0.0
        graphs[i].set("weightedMean", f"{weightedMeanSum:.6e}")

        error1=graphs[i].get('error')
        error2=graphs2[i].get('error')
        errorSum = (float(error1) ** 2 + float(error2) ** 2) ** 0.5
        graphs[i].set("error", f"{errorSum:.6e}")

    for graph in graphs:
        if int(graph.get("entries", "0")) == 0:
            data1.remove(graph)
    for graph in graphs2:
        if int(graph.get("entries", "0")) == 0:
            data2.remove(graph)


tree1.write(args.outputfilename, encoding="UTF-8", xml_declaration=True)