#! /usr/bin/env python3
import serial, csv, sys, time


#Arduinoのシリアルポート作成
ser = serial.Serial('/dev/cu.usbmodem1421',9600,timeout=None)
time.sleep(2)

#結果をExcelに入れるためのリスト作成
subject_list = [[None for j in range(14)] for i in range(12)]
subject_list[0][0] = "被験者"
subject_list[0][3] = "黄色の加速度"
subject_list[0][7] = "緑の加速度"
subject_list[0][11] = "赤の加速度"
subject_list[1][3]=subject_list[1][7]=subject_list[1][11] = "X軸"
subject_list[1][4]=subject_list[1][8]=subject_list[1][12] = "Y軸"
subject_list[1][5]=subject_list[1][9]=subject_list[1][13] = "Z軸"
side_line = 0
length_line = 1



#規則的に反応時間をExcelに入れるためのcsvリストを作る
def create_list(line):
    global accele_length_line
    global side_line
    global length_line
#反応時間をリストに入れる
    if subject_list[8][1] == None:
        subject_list[length_line][side_line] = line
        side_line += 1
        answer = side_line / 2
        side_line = side_line % 2
        if answer >= 1 and side_line == 0:
            length_line += 1
#黄色の加速度をリストを入れる
    elif subject_list[8][1] != None and subject_list[accele_length_line - 1][5] == None:
        if subject_list[2][3] == None:
            length_line = 2
            side_line = 3
        length_line, side_line = accele_create_list(line, length_line, side_line)
        #yellow_length_line, yellow_side_line = accele_create_list(line, yellow_length_line, yellow_side_line)
#緑の加速度をリストを入れる
    elif subject_list[8][1] != None and subject_list[accele_length_line - 1][5] != None and subject_list[accele_length_line - 1][9] == None:
        if subject_list[2][7] == None:
            length_line = 2
            side_line = 7
        length_line, side_line = accele_create_list(line, length_line, side_line)
#赤の加速度をリストを入れる
    elif subject_list[8][1] != None and subject_list[accele_length_line - 1][5] != None and subject_list[accele_length_line - 1][9] != None and subject_list[accele_length_line - 1][13] == None:
        if subject_list[2][11] == None:
            length_line = 2
            side_line = 11
        length_line, side_line = accele_create_list(line, length_line, side_line)

#加速度をいれるための形式の関数
def accele_create_list(line, local_length, local_side):
    global accele_length_line
    subject_list[local_length][local_side] = line
    local_length += 1
    answer = local_length / accele_length_line
    local_length = local_length % accele_length_line
    if answer >= 1 and local_length == 0:
        local_side += 1
        local_length += 2
    return local_length, local_side


#結果をExcelに実際に入れる関数
def excel_write(file_name):
    with open("subject" + file_name + ".csv", "w", encoding='shift_jis') as csv_file:
        subject_file = csv.writer(csv_file)
        subject_file.writerows(subject_list)
        #subject_file.close()
        #print(subject_list[8][1]) #最後がきちんとはいっているか確認

#コマンドラインの引数の確認とArduinoとのシリアル通信のやりとり
def main():
    global accele_length_line
    print("1:12回, 2:15回, 3:18回, 4:21回, 5:24回")
    if len(sys.argv) == 3:
        try:
            int_number = int(sys.argv[2])
            if 0 <= int_number and int_number <= 5:
                ser.write(sys.argv[2].encode('ascii'))
                file_name = sys.argv[1]
                accele_length_line = int(sys.argv[2]) + 5


                while True:
                    line = ser.readline()
                    line = line.decode('utf-8')
                    line = line.rstrip()
                    create_list(line)

                    if subject_list[8][1] != None and subject_list[accele_length_line - 1][5] != None and subject_list[accele_length_line - 1][9] != None and subject_list[accele_length_line - 1][13] != None:
                        excel_write(file_name)
                        print("File created")
                        break
            else:
                print("ただしい整数を入力してください")
        except ValueError:
            print("ただしい整数を入力して下さい")

    else:
        print("一つ目の引数にファイル名、二つめの引数に回数をいれてください")
    ser.close()


if __name__ == "__main__":
    main()
