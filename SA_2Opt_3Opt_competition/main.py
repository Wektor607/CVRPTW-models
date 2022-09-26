from twMethods import *
from collections import deque
from datetime import datetime
import tools
import numpy as np
import pandas as pd
def translate_minute(sec):
    minute = sec // 60
    hour = minute // 60 
    minute %= 60 
    return "%02d:%02d" % (hour, minute)

def main():

    lst = [
    "instances/ORTEC-VRPTW-ASYM-1f1fffc4-d1-n332-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-2aa3e5eb-d1-n239-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-2c4cceaa-d1-n470-k35.txt",
    "instances/ORTEC-VRPTW-ASYM-2cbc6e34-d1-n454-k35.txt",
    "instances/ORTEC-VRPTW-ASYM-2d8735ce-d1-n270-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-2e0a30ff-d1-n491-k36.txt",
    "instances/ORTEC-VRPTW-ASYM-2e2ef021-d1-n210-k17.txt",
    "instances/ORTEC-VRPTW-ASYM-3cc58e3a-d1-n345-k22.txt",
    "instances/ORTEC-VRPTW-ASYM-3ce62772-d1-n241-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-3eea4ee3-d1-n382-k23.txt",
    "instances/ORTEC-VRPTW-ASYM-3ff97275-d1-n426-k30.txt",
    "instances/ORTEC-VRPTW-ASYM-04c694cd-d1-n254-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-4b81ac13-d1-n288-k30.txt",
    "instances/ORTEC-VRPTW-ASYM-4b1392b2-d1-n338-k21.txt",
    "instances/ORTEC-VRPTW-ASYM-4c69f727-d1-n204-k12.txt",
    "instances/ORTEC-VRPTW-ASYM-4cf3d422-d1-n238-k30.txt",
    "instances/ORTEC-VRPTW-ASYM-4d88728c-d1-n424-k40.txt",
    "instances/ORTEC-VRPTW-ASYM-4da724b1-d1-n393-k30.txt",
    "instances/ORTEC-VRPTW-ASYM-4e5f1a3a-d1-n205-k17.txt",
    "instances/ORTEC-VRPTW-ASYM-4e39e33e-d1-n331-k31.txt",
    "instances/ORTEC-VRPTW-ASYM-4f179ccb-d1-n416-k26.txt",
    "instances/ORTEC-VRPTW-ASYM-4fe9435f-d1-n418-k28.txt",
    "instances/ORTEC-VRPTW-ASYM-5b9c2a77-d1-n284-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-5c968166-d1-n320-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-5d8fd227-d1-n211-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-5e5904f3-d1-n203-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-5fa16580-d1-n329-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-6ac64141-d1-n254-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-6aedfe61-d1-n308-k35.txt",
    "instances/ORTEC-VRPTW-ASYM-6c6190a5-d1-n323-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-6d6b508e-d1-n413-k35.txt",
    "instances/ORTEC-VRPTW-ASYM-6ed7e774-d1-n313-k19.txt",
    "instances/ORTEC-VRPTW-ASYM-6f54fdb4-d1-n392-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-6f389522-d1-n253-k23.txt",
    "instances/ORTEC-VRPTW-ASYM-7a80cc21-d1-n214-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-7a65458b-d1-n326-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-7b8058ef-d1-n299-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-7ca2d255-d1-n339-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-7d3e0a2b-d1-n307-k28.txt",
    "instances/ORTEC-VRPTW-ASYM-7d333c6d-d1-n338-k24.txt",
    "instances/ORTEC-VRPTW-ASYM-7e181d8a-d1-n305-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-7e105293-d1-n268-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-7ebadc2f-d1-n237-k35.txt",
    "instances/ORTEC-VRPTW-ASYM-7ef1213a-d1-n292-k23.txt",
    "instances/ORTEC-VRPTW-ASYM-7f99f05a-d1-n528-k40.txt",
    "instances/ORTEC-VRPTW-ASYM-7f162df6-d1-n293-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-7f658fd0-d1-n335-k22.txt",
    "instances/ORTEC-VRPTW-ASYM-7fa45da1-d1-n316-k19.txt",
    "instances/ORTEC-VRPTW-ASYM-7fb5f610-d1-n532-k40.txt",
    "instances/ORTEC-VRPTW-ASYM-08d8e660-d1-n460-k42.txt",
    "instances/ORTEC-VRPTW-ASYM-8a61f4e1-d1-n210-k12.txt",
    "instances/ORTEC-VRPTW-ASYM-8b1620d9-d1-n346-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-8bc1fa17-d1-n302-k23.txt",
    "instances/ORTEC-VRPTW-ASYM-8bc13a3f-d1-n421-k40.txt",
    "instances/ORTEC-VRPTW-ASYM-8dfa33df-d1-n328-k31.txt",
    "instances/ORTEC-VRPTW-ASYM-9b290a2e-d1-n403-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-9ba5962e-d1-n329-k23.txt",
    "instances/ORTEC-VRPTW-ASYM-9c08a0ac-d1-n250-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-9d043e37-d1-n205-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-9d100d94-d1-n409-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-9ee807e9-d1-n248-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-9faccacb-d1-n397-k22.txt",
    "instances/ORTEC-VRPTW-ASYM-9ffcb659-d1-n425-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-13db18b2-d1-n310-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-16b82253-d1-n457-k30.txt",
    "instances/ORTEC-VRPTW-ASYM-19eafe1e-d1-n458-k35.txt",
    "instances/ORTEC-VRPTW-ASYM-21e8376e-d1-n507-k30.txt",
    "instances/ORTEC-VRPTW-ASYM-26e86ad5-d1-n288-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-27e27157-d1-n350-k22.txt",
    "instances/ORTEC-VRPTW-ASYM-30d4e457-d1-n290-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-31c42adc-d1-n206-k12.txt",
    "instances/ORTEC-VRPTW-ASYM-31d5cfe6-d1-n252-k30.txt",
    "instances/ORTEC-VRPTW-ASYM-35fbecef-d1-n220-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-36b75313-d1-n225-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-37bce731-d1-n239-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-38e65f4b-d1-n443-k35.txt",
    "instances/ORTEC-VRPTW-ASYM-50d1f78d-d1-n329-k19.txt",
    "instances/ORTEC-VRPTW-ASYM-50d3c13c-d1-n335-k28.txt",
    "instances/ORTEC-VRPTW-ASYM-52f4e342-d1-n301-k16.txt",
    "instances/ORTEC-VRPTW-ASYM-53ac3334-d1-n285-k21.txt",
    "instances/ORTEC-VRPTW-ASYM-55a26fb1-d1-n326-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-56c07872-d1-n309-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-60a623ab-d1-n329-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-64f72f5d-d1-n225-k16.txt",
    "instances/ORTEC-VRPTW-ASYM-68e134ef-d1-n267-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-71cec69e-d1-n277-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-77a8b324-d1-n330-k23.txt",
    "instances/ORTEC-VRPTW-ASYM-77e75b62-d1-n227-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-83a6eaf5-d1-n225-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-83be8e48-d1-n350-k21.txt",
    "instances/ORTEC-VRPTW-ASYM-87afa1b5-d1-n296-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-90e8a20e-d1-n296-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-91a5c759-d1-n263-k35.txt",
    "instances/ORTEC-VRPTW-ASYM-92bc6975-d1-n273-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-93c37e02-d1-n430-k35.txt",
    "instances/ORTEC-VRPTW-ASYM-93ee144d-d1-n688-k38.txt",
    "instances/ORTEC-VRPTW-ASYM-94beb4a8-d1-n371-k21.txt",
    "instances/ORTEC-VRPTW-ASYM-94f07625-d1-n254-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-97d6ff30-d1-n328-k22.txt",
    "instances/ORTEC-VRPTW-ASYM-152bab99-d1-n215-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-293f7c12-d1-n237-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-359b0530-d1-n318-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-574c9314-d1-n288-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-617e20d7-d1-n203-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-640de60f-d1-n318-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-752be4a9-d1-n372-k24.txt",
    "instances/ORTEC-VRPTW-ASYM-0797afaf-d1-n313-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-839f6b13-d1-n265-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-844ea26c-d1-n231-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-898e14bd-d1-n230-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-937b2edc-d1-n234-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-945a5617-d1-n355-k27.txt",
    "instances/ORTEC-VRPTW-ASYM-02182cf8-d1-n327-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-3709cf41-d1-n287-k19.txt",
    "instances/ORTEC-VRPTW-ASYM-4285a426-d1-n450-k28.txt",
    "instances/ORTEC-VRPTW-ASYM-5525d5a9-d1-n255-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-6199a008-d1-n220-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-6563cca6-d1-n381-k28.txt",
    "instances/ORTEC-VRPTW-ASYM-6570ec5a-d1-n283-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-6650f64e-d1-n586-k32.txt",
    "instances/ORTEC-VRPTW-ASYM-6656ccd2-d1-n310-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-8083ca8d-d1-n313-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-8512fc31-d1-n201-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-8971de71-d1-n558-k34.txt",
    "instances/ORTEC-VRPTW-ASYM-9016f313-d1-n200-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-9219f367-d1-n252-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-33202ac2-d1-n259-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-40856d69-d1-n217-k12.txt",
    "instances/ORTEC-VRPTW-ASYM-53690b3c-d1-n321-k35.txt",
    "instances/ORTEC-VRPTW-ASYM-57242a8c-d1-n309-k19.txt",
    "instances/ORTEC-VRPTW-ASYM-62980c46-d1-n254-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-715940dc-d1-n381-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-798171b0-d1-n273-k21.txt",
    "instances/ORTEC-VRPTW-ASYM-886856a8-d1-n455-k45.txt",
    "instances/ORTEC-VRPTW-ASYM-971718c6-d1-n220-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-01829532-d1-n324-k22.txt",
    "instances/ORTEC-VRPTW-ASYM-7101755b-d1-n514-k31.txt",
    "instances/ORTEC-VRPTW-ASYM-7610508a-d1-n230-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-11527044-d1-n470-k27.txt",
    "instances/ORTEC-VRPTW-ASYM-37572525-d1-n240-k16.txt",
    "instances/ORTEC-VRPTW-ASYM-41880837-d1-n262-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-82340194-d1-n270-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-a2c1e21d-d1-n504-k38.txt",
    "instances/ORTEC-VRPTW-ASYM-a6f1b493-d1-n299-k21.txt",
    "instances/ORTEC-VRPTW-ASYM-a9d4edc2-d1-n200-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-a53ed404-d1-n406-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-a53f89ac-d1-n234-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-a59d64ac-d1-n328-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-a69bcc0c-d1-n249-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-a631ce65-d1-n214-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-a1081c7b-d1-n325-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-aa746fc7-d1-n478-k45.txt",
    "instances/ORTEC-VRPTW-ASYM-ab61546c-d1-n265-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-acc237bb-d1-n390-k23.txt",
    "instances/ORTEC-VRPTW-ASYM-addee14a-d1-n302-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-ae0802e0-d1-n268-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-aecf9ffb-d1-n420-k26.txt",
    "instances/ORTEC-VRPTW-ASYM-af03d3bd-d1-n214-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-afb4cbe2-d1-n457-k45.txt",
    "instances/ORTEC-VRPTW-ASYM-b0db3a58-d1-n220-k13.txt",
    "instances/ORTEC-VRPTW-ASYM-b6aa1919-d1-n212-k13.txt",
    "instances/ORTEC-VRPTW-ASYM-b7ae3aa2-d1-n304-k23.txt",
    "instances/ORTEC-VRPTW-ASYM-b35f2931-d1-n212-k13.txt",
    "instances/ORTEC-VRPTW-ASYM-b201abbb-d1-n360-k24.txt",
    "instances/ORTEC-VRPTW-ASYM-b345b203-d1-n215-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-b384a276-d1-n219-k17.txt",
    "instances/ORTEC-VRPTW-ASYM-b3059fbe-d1-n325-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-b3635658-d1-n457-k28.txt",
    "instances/ORTEC-VRPTW-ASYM-bbed1122-d1-n347-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-bc44b197-d1-n296-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-bda861e7-d1-n364-k22.txt",
    "instances/ORTEC-VRPTW-ASYM-bdfcbd22-d1-n247-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-c0f56186-d1-n499-k31.txt",
    "instances/ORTEC-VRPTW-ASYM-c15b0201-d1-n320-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-c67b3a3f-d1-n342-k21.txt",
    "instances/ORTEC-VRPTW-ASYM-c82ca041-d1-n393-k23.txt",
    "instances/ORTEC-VRPTW-ASYM-c853ff0d-d1-n236-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-c7657e35-d1-n275-k21.txt",
    "instances/ORTEC-VRPTW-ASYM-ca1ed34e-d1-n226-k21.txt",
    "instances/ORTEC-VRPTW-ASYM-ca5d17ad-d1-n246-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-cc98aff5-d1-n261-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-cc815064-d1-n470-k26.txt",
    "instances/ORTEC-VRPTW-ASYM-ccc004aa-d1-n224-k12.txt",
    "instances/ORTEC-VRPTW-ASYM-ce78488d-d1-n332-k30.txt",
    "instances/ORTEC-VRPTW-ASYM-d0b8103f-d1-n299-k21.txt",
    "instances/ORTEC-VRPTW-ASYM-d0d70aec-d1-n290-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-d3fb97ea-d1-n212-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-d5a2598f-d1-n344-k22.txt",
    "instances/ORTEC-VRPTW-ASYM-d8fb9efc-d1-n237-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-d9af647d-d1-n237-k16.txt",
    "instances/ORTEC-VRPTW-ASYM-d57da137-d1-n222-k14.txt",
    "instances/ORTEC-VRPTW-ASYM-d83ecd50-d1-n399-k30.txt",
    "instances/ORTEC-VRPTW-ASYM-d98c5742-d1-n204-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-d137e14b-d1-n257-k17.txt",
    "instances/ORTEC-VRPTW-ASYM-d708f094-d1-n300-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-d47924f8-d1-n225-k14.txt",
    "instances/ORTEC-VRPTW-ASYM-d1906538-d1-n226-k30.txt",
    "instances/ORTEC-VRPTW-ASYM-da200d12-d1-n317-k17.txt",
    "instances/ORTEC-VRPTW-ASYM-db776df3-d1-n322-k23.txt",
    "instances/ORTEC-VRPTW-ASYM-db8794fb-d1-n319-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-dd43a785-d1-n880-k50.txt",
    "instances/ORTEC-VRPTW-ASYM-dd59b788-d1-n252-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-deb1b4a4-d1-n457-k29.txt",
    "instances/ORTEC-VRPTW-ASYM-dec17385-d1-n268-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-e2f2ccf7-d1-n285-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-e03a17e0-d1-n406-k21.txt",
    "instances/ORTEC-VRPTW-ASYM-e3f9f583-d1-n286-k16.txt",
    "instances/ORTEC-VRPTW-ASYM-e6de598f-d1-n277-k16.txt",
    "instances/ORTEC-VRPTW-ASYM-e32e5e1a-d1-n214-k12.txt",
    "instances/ORTEC-VRPTW-ASYM-e59eea41-d1-n266-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-e87e8a3a-d1-n235-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-e92a0b44-d1-n501-k33.txt",
    "instances/ORTEC-VRPTW-ASYM-e159d033-d1-n363-k27.txt",
    "instances/ORTEC-VRPTW-ASYM-e260d9dc-d1-n480-k45.txt",
    "instances/ORTEC-VRPTW-ASYM-e785c231-d1-n291-k16.txt",
    "instances/ORTEC-VRPTW-ASYM-e814fc2f-d1-n247-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-e4647cbd-d1-n262-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-e7835db2-d1-n650-k42.txt",
    "instances/ORTEC-VRPTW-ASYM-e8628692-d1-n296-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-ed97d7dc-d1-n416-k23.txt",
    "instances/ORTEC-VRPTW-ASYM-edb5bbc1-d1-n350-k24.txt",
    "instances/ORTEC-VRPTW-ASYM-ef1c4bb9-d1-n236-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-ef7dad5e-d1-n200-k12.txt",
    "instances/ORTEC-VRPTW-ASYM-ef56c080-d1-n327-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-efe3fa8e-d1-n346-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-effcce2d-d1-n408-k20.txt",
    "instances/ORTEC-VRPTW-ASYM-f2de4908-d1-n403-k34.txt",
    "instances/ORTEC-VRPTW-ASYM-f3d74014-d1-n238-k13.txt",
    "instances/ORTEC-VRPTW-ASYM-f5e6799e-d1-n311-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-f77bf975-d1-n342-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-f80d8c50-d1-n208-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-f216c483-d1-n399-k28.txt",
    "instances/ORTEC-VRPTW-ASYM-f346bd6e-d1-n353-k21.txt",
    "instances/ORTEC-VRPTW-ASYM-f854cc6f-d1-n299-k23.txt",
    "instances/ORTEC-VRPTW-ASYM-fa18d4ba-d1-n349-k30.txt",
    "instances/ORTEC-VRPTW-ASYM-fb19b5fc-d1-n274-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-fb43d5a5-d1-n263-k17.txt",
    "instances/ORTEC-VRPTW-ASYM-fd08dfcd-d1-n309-k19.txt",
    "instances/ORTEC-VRPTW-ASYM-fd32ebe8-d1-n201-k15.txt",
    "instances/ORTEC-VRPTW-ASYM-fec88673-d1-n302-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-fee7a003-d1-n329-k18.txt",
    "instances/ORTEC-VRPTW-ASYM-ffa0f3b4-d1-n363-k22.txt",
    "instances/ORTEC-VRPTW-ASYM-00c5356f-d1-n258-k12.txt",
    "instances/ORTEC-VRPTW-ASYM-0bdff870-d1-n458-k35.txt",
    "instances/ORTEC-VRPTW-ASYM-0dc59ef2-d1-n213-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-1a452a2c-d1-n391-k23.txt",
    "instances/ORTEC-VRPTW-ASYM-1bc16246-d1-n503-k43.txt",
    "instances/ORTEC-VRPTW-ASYM-1bdf25a7-d1-n531-k43.txt",
    "instances/ORTEC-VRPTW-ASYM-1cd538a9-d1-n400-k25.txt",
    "instances/ORTEC-VRPTW-ASYM-1de83915-d1-n262-k15.txt"
    ]

    print('which method do you want to use to optimize the routse for CVRPTW: SA or opt?')
    method = input()
    idx = 0
    if(method == 'SA'):
        for i_lst in range(len(lst)):
            data = tools.read_vrplib(lst[i_lst])
            with open(f'parse_instance/example{i_lst}.csv','w') as fout:
                fout.write('x_coord' + '\t' + 'y_coord' + '\t' + 'Weight' + '\t' + 'DeliveryTimeRange' + '\t' + 'UnloadingTime' + '\n')
                for i in range(1,len(data['coords'])):
                    fout.write(np.array2string(data['coords'][i][0]) + '\t' + np.array2string(data['coords'][i][1])  + '\t' + np.array2string(data['demands'][i])  + '\t' + translate_minute(data['time_windows'][i][0]) + '-' + translate_minute(data['time_windows'][i][1]) + '\t' + np.array2string(data['service_times'][i]) + '\n')
                fout.write(np.array2string(data['coords'][0][0]) + '\t' + np.array2string(data['coords'][0][1])  + '\t' + np.array2string(data['demands'][0])  + '\t' + translate_minute(data['time_windows'][0][0]) + '-' + translate_minute(data['time_windows'][0][1]) + '\t' + np.array2string(data['service_times'][0]) + '\n')    
            
            nf = f'parse_instance/example{i_lst}.csv'
            d = pd.DataFrame(data['duration_matrix'])
            d.to_csv(f'parse_instance/time_matrix_{i_lst}.csv',index = False,sep = '\t', columns = None)
            mnf = f'parse_instance/time_matrix_{i_lst}.csv'
            count_towns = len(data['is_depot'])
            max_capacity = data['capacity']
            iteretions   = 2500
            start = data['time_windows'][0][0]
            end = data['time_windows'][0][1]
            a = CVRPTW('SA', nf, mnf,  f"comp_tw/test{idx}", count_towns, iteretions, max_capacity, start, end) #TODO: некоторые параметры брать автоматически из файла
            idx += 1
            print(a.sa())
            with open(f'SA_CVRPTW_result.txt', 'r') as res_file:
                data = res_file.read()
                with open(f'SA_FULL_RES_CVRPTW.txt', 'a') as write_file:
                    write_file.write(data)
    elif(method == 'opt'):
        for i_lst in range(len(lst)):
            data = tools.read_vrplib(lst[i_lst])
            with open(f'parse_instance/example{i_lst}.csv','w') as fout:
                fout.write('x_coord' + '\t' + 'y_coord' + '\t' + 'Weight' + '\t' + 'DeliveryTimeRange' + '\t' + 'UnloadingTime' + '\n')
                for i in range(1,len(data['coords'])):
                    fout.write(np.array2string(data['coords'][i][0]) + '\t' + np.array2string(data['coords'][i][1])  + '\t' + np.array2string(data['demands'][i])  + '\t' + translate_minute(data['time_windows'][i][0]) + '-' + translate_minute(data['time_windows'][i][1]) + '\t' + np.array2string(data['service_times'][i]) + '\n')
                fout.write(np.array2string(data['coords'][0][0]) + '\t' + np.array2string(data['coords'][0][1])  + '\t' + np.array2string(data['demands'][0])  + '\t' + translate_minute(data['time_windows'][0][0]) + '-' + translate_minute(data['time_windows'][0][1]) + '\t' + np.array2string(data['service_times'][0]) + '\n')    
            
            nf = f'parse_instance/example{i_lst}.csv'
            d = pd.DataFrame(data['duration_matrix'])
            d.to_csv(f'parse_instance/time_matrix_{i_lst}.csv',index = False,sep = '\t', columns = None)
            mnf = f'parse_instance/time_matrix_{i_lst}.csv'
            while(1):
                print("Input 2opt or 3opt:")
                name_opt = input()
                if(name_opt == '2opt' or name_opt == '3opt' or name_opt == 'lkh'):
                    break
                else:
                    print('Try again!')
            
            count_towns = len(data['is_depot'])
            max_capacity = data['capacity']
            iteretions   = 2500
            start = data['time_windows'][0][0]
            end = data['time_windows'][0][1]
            a = CVRPTW('opt', nf, mnf,  f"comp_tw/test{idx}", count_towns, iteretions, max_capacity, start, end)#TODO: некоторые параметры брать автоматически из файла
            idx += 1
            print(a.opt(name_opt))
            if(name_opt == '2opt' or name_opt == '3opt'):
                with open(f'LKH_{name_opt}_CVRPTW_result.txt', 'r') as res_file:
                    data = res_file.read()
                    with open(f'LKH_{name_opt}_FULL_RES_CVRPTW.txt', 'a') as write_file:
                        write_file.write(data)
            else:
                with open(f'LKH_CVRPTW_result.txt', 'r') as res_file:
                    data = res_file.read()
                    with open(f'LKH_FULL_RES_CVRPTW.txt",', 'a') as write_file:
                        write_file.write(data)
        
if __name__ == "__main__":
    main()