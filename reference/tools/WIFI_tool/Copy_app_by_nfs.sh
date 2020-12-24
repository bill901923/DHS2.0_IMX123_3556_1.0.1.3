                        
addrout="/mnt/reference/out/hi3559v200_dashcam_carrecorder_refb_imx123/bin/emmc"                                                     
Doaddrout=""  
#kill -9 `pidof wpa_supplicant`
#kill -9 `pidof DHS_app`

#insmod /app/komod/8189fs.ko
#sh /app/8189_wifi.sh sta open
 
copedata()                                               
{                                                         
        echo $1;                                          
        if [ "$1" = "main" ]                                
        then    
                rm  /app/bin/main_app
                Doaddrout=$addrout"/linux/main_app /app/bin/main_app"         
                
                cp $Doaddrout                                 
        elif [ "$1" = "DHS" ]                                                    
        then      
                rm  /app/bin/DHS_app
                Doaddrout=$addrout"/DHS/DHS_app /app/bin/DHS_app"         
                
                cp $Doaddrout                                 
        elif [ "$1" = "TEST" ]                                                    
        then      
                rm  /app/bin/TEST_app
                Doaddrout=$addrout"/TEST/TEST_app /app/bin/TEST_app"         
                
                cp $Doaddrout                                                  
        else                                                                         
                echo "copedata():should main or DHS"                                 
        fi                                                
}                                                                       
if [ $# != 1 ]                                                          
then                                                                                 
        echo "param error!"                                                          
else       
        mount -v -t nfs -o nolock 192.168.1.14:/home/xll/workspace/DHS2.0_IMX123_3556_1.0.1.3 /mnt                                                                           
        copedata $1                                                    
fi               
