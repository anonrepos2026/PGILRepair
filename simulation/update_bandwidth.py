# script/update_pivot_xml.py
import xml.etree.ElementTree as ET

BW_FILE = "bw_snapshot.txt"
XML_FILE = "conf/sysSetting.xml"

ul = []
dl = []

with open(BW_FILE) as f:
    for line in f:
        u = int(line.split("UL[")[1].split("]=")[1].split()[0])
        d = int(line.split("DL[")[1].split("]=")[1])
        ul.append(str(u))
        dl.append(str(d))

ul_str = ",".join(ul)
dl_str = ",".join(dl)

print("[DEBUG] Writing ul_con: ", ul_str);
print("[DEBUG] Writing dl_con: ", dl_str);

tree = ET.parse(XML_FILE)
root = tree.getroot()

for attr in root.findall("attribute"):
    name = attr.find("name")
    if name is not None and name.text == "ec.policy":
        for val in attr.findall("value"):
            ecid = val.find("ecid")
            if ecid is not None and ecid.text == "rs_pivot":
                val.find("ul_con").text = ul_str
                val.find("dl_con").text = dl_str
                print("[INFO] rs_pivot BW updated")

tree.write(XML_FILE)
