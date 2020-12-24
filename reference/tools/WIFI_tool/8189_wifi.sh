#cd /app/kmod                              
                                                                                       
                                                          
#mv /dev/random /dev/random.orig                          
#ln -s /dev/urandom /dev/random     
libnl_genl_3="/lib/libnl-genl.so.3"    
libnl_genl_2="/lib/libnl-genl.so.2"     
libnl_3="/lib/libnl.so.3"     
libnl_2="/lib/libnl.so.2" 

if [ ! -f "$libnl_genl_3" ]; then
        cp lib/libnl-genl.so.3.0.0 "$libnl_genl_3"
fi     

if [ ! -f "$libnl_genl_2" ]; then     
cp lib/libnl-genl.so.3.0.0 "$libnl_genl_2"
fi  

if [ ! -f "$libnl_3" ]; then          
cp lib/libnl.so.3.0.0 "$libnl_3"
fi  

if [ ! -f "$libnl_2" ]; then
cp lib/libnl.so.3.0.0 "$libnl_2" 
fi                                              
                                                        
open_wifi()                                               
{                                                         
        echo $1;                                          
        if [ "$1" = "ap" ]                                
        then                                              
                ifconfig wlan0 192.168.0.1 up             
                /app/wireless/hostapd -e /app/wireless/entropy.bin /app/wireless/hostapd.conf &
                udhcpd /app/wireless/udhcpd.conf                                 
        elif [ "$1" = "sta" ]                                                    
        then                                                                     
                ifconfig wlan0 up                                                
                /app/wireless/wpa_supplicant -iwlan0 -Dnl80211 -c/app/wireless/wpa_supplicant.conf&
                #ifconfig wlan0 192.168.0.111  
                ifconfig wlan0 192.168.1.111 
                #ifconfig wlan0 192.168.43.175                                       
                #udhcpc -i wlan0                                                      
        else                                                                         
                echo "open_wifi():should ap or sta!"                                 
        fi                                                
}                                                         
close_wifi()                                                                         
{                                                                                    
        echo $1;                                                                     
        if [ "$1" = "ap" ]                      
        then                                        
                ifconfig wlan0 down && killall hostapd && killall udhcpd             
        elif [ "$1" = "sta" ]                                                        
        then                                                                         
                ifconfig wlan0 down && killall wpa_supplicant && killall udhcpc  
        else                                                                         
                echo "close_wifi():should ap or sta!"                                
        fi                                                                           
                                                                                     
}                                                                                    
reset_wifi()                                                                         
{                                                                                    
        close_wifi $1                                                                
        sleep 2                                                                      
        open_wifi $1                                                                 
}                                                                       
if [ $# != 2 ]                                                          
then                                                                                 
        echo "param error!"                                                          
else                                                                                 
        case $2 in                                                             
                open)                                                          
                        #set_up_wifi $1                                               
                        open_wifi $1                                                 
                ;;                                                                   
                close) close_wifi $1                                             
                ;;                                                               
                reset) reset_wifi $1                                                 
        esac                                                                         
fi               
