import pandas as pd
import datetime
import os



class StateMng:
    def __init__(self):
        self.data_dict = {
            "1": ["rx1_", "rx1_mask","rx1_filter"],
            "2": ["rx2_", "rx2_mask","rx2_filter"],
            "3": ["rx3_", "rx3_mask","rx3_filter"],
            "4": ["rx4_", "rx4_mask","rx4_filter"],
            "5": ["rx5_", "rx5_mask","rx5_filter"],
            "6": ["rx6_", "rx6_mask", "rx6_filter"],
            "7": ["rx7_", "rx7_mask", "rx7_filter"],
        }

        print("init...")

    def Get_mask_filter(self,data1,data2,mask,filter):
        table = {
            0: 0x001,
            1: 0x002,
            2: 0x004,
            3: 0x008,

            4: 0x010,
            5: 0x020,
            6: 0x040,
            7: 0x080,

            8: 0x100,
            9: 0x200,
            10:0x400,
        }



        for key in table:
            if table[key] & data1 == table[key] & data2:
                if table[key] & data1 == 0:
                    filter = filter & ~table[key];
                else:
                    pass
            else:
                mask = mask & ~table[key];
                filter = filter & ~table[key];


        return mask,filter



if __name__ == "__main__":
    StateMngObj = StateMng()
    curpath = os.getcwd()
    fullpath = curpath + '\\' + "cal_canfilter.csv"
    print(fullpath)
    df = pd.read_csv(fullpath)
    print(df)
    tempval = 0
    for key in StateMngObj.data_dict:
        mask = 0x7FF
        filter = 0x7FF
        tempval = 0
        for index in df.index:
            if pd.isnull(df.loc[index, StateMngObj.data_dict[key][0]]):
                break
            if index == 0:
                tempval = int(df.loc[index, StateMngObj.data_dict[key][0]])
                mask, filter = StateMngObj.Get_mask_filter(tempval, int(df.loc[index, StateMngObj.data_dict[key][0]]),mask, filter)
            else:
                mask,filter = StateMngObj.Get_mask_filter(tempval,int(df.loc[index, StateMngObj.data_dict[key][0]]),mask,filter)
                print(hex(mask),hex(filter),hex(tempval),hex(int(df.loc[index, StateMngObj.data_dict[key][0]])))
        df.loc[0, StateMngObj.data_dict[key][1]] = hex(mask)
        df.loc[0, StateMngObj.data_dict[key][2]] = hex(filter)

    print(df)

    fullpath = curpath + '\\' + "cal_canfilter_gen.csv"
   # pd.to_csv(fullpath)
    df.to_csv(fullpath, index=0)

    print("close file success...")





#打包命令
#Pyinstaller -F -w -i chengzi.ico py_word.py
#Pyinstaller -F -c -i chengzi.ico py_word.py