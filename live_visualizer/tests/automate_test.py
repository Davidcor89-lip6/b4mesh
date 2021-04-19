import threading 
import time
import os
import os.path
import sys

input_file_path = './datas/blockgraph.dot.json'
output_file_dir = './datas/tmp/'
output_file_path = output_file_dir + 'TestDatas.json'

print("Starting...", flush=True)
print("Press <ctrl+c> to quit ...", flush=True)

try:
    if not os.path.isdir(output_file_dir):
        os.mkdir(output_file_dir)
except OSError:
    print ("Creation of the directory %s failed" % path)
    raise

def line_generator(input_file_path, output_file_path):
    try:
        open(output_file_path, 'w').close() # clean output file
        with open(input_file_path, 'r') as datas_file:
            datas = datas_file.readlines()
            #while True:
            #print("looping ...", flush=True)
            for line in datas:
                print(" - line : {}".format(line.strip()), flush=True)
                with open(output_file_path, 'a+') as output_file:
                    output_file.write(line)
                    time.sleep(3)
            
    except OSError as err:
        print("OS error: {0}".format(err), flush=True)
    except ValueError:
        print("Could not convert data to an integer.", flush=True)
    except:
        print("Unexpected error:", sys.exc_info()[0], flush=True)
        raise

class LineGeneratorThread(threading.Thread):
    def __init__(self, *args, **keywords):
        threading.Thread.__init__(self, *args, **keywords)
        self._stop = threading.Event() 

    def stop(self): 
        self._stop.set() 
  
    def stopped(self): 
        return self._stop.isSet() 

    def run(self):
        try:
            with open(input_file_path, 'r') as datas_file:
                datas = datas_file.readlines()
                while True:
                    open(output_file_path, 'w').close() # clean output file
                    for line in datas:
                        if self.stopped(): 
                            return
                        print(" - line : {}".format(line.strip()), flush=True)
                        with open(output_file_path, 'a+') as output_file:
                            output_file.write(line)
                            time.sleep(3)
        except OSError as err:
            print("OS error: {0}".format(err), flush=True)
        except:
            print("Unexpected error:", sys.exc_info()[0], flush=True)
            raise

        

task = LineGeneratorThread()
task.daemon = True
task.start()

try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    print("Quit requested ...", flush=True)
    task.stop()

print("Finishing ...", flush=True)
task.join()

if os.path.isfile(output_file_path):
    os.rm(output_file_dir)
if os.path.isdir(output_file_dir):
    os.rmdir(output_file_path)

sys.exit()