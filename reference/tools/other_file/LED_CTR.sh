#cd /app/kmod                              
                                                                                       
                                                          
#mv /dev/random /dev/random.orig                          
#ln -s /dev/urandom /dev/random                           
                                                      
#himm 0x120101BC		0x01807983		//PERI_CRG111 50M 0x01807883 （3M）  

#LED_PWM13
#himm 0x111F0028		   0x631	  //iocfg_reg61 PWM1/GPIO6_7 0x631
#himm 0x12070020 		3571	//PWM0_CFG0 PWM0 的配置 0  50M/14k = 3571
#himm 0x12070024 		0		//PWM0_CFG1 PWM0 的配置 1 
#himm 0x1207002C 		0x5		//PWM0_CTRL PWM0 的控制

#LED_PWM2		
#himm 0x111F0024		0x631	//iocfg_reg60 PWM0/GPIO6_6 0x631 
#himm 0x12070000 		3571	//PWM0_CFG0 PWM0 的配置 0  50M/14k = 3571
#himm 0x12070004 		0		//PWM0_CFG1 PWM0 的配置 1 
#himm 0x1207000C 		0x5		//PWM0_CTRL PWM0 的控制   
#LED_P_EN 		
#himm 0x114F0058		0x0 //iocfg_reg40  VI_DATA6/GPIO3_6/VOU_DATA6
#himm 0x120D3400     		0x40
#himm 0x120D3100     		0x40
#himm 0x120D3100     		0  
                                                   
open_LED()                                               
{          
        himm 0x120D3100 0   
	himm 0x120D3400 0x40                                            
        echo $1;                                          
        if [ "$1" = "LED0" ]                                
        then                                              
               himm 0x111F0028 0x631
               himm 0x12070020 3571
               himm 0x12070024 $3	
               himm 0x1207002C 0x5	
                               
        elif [ "$1" = "LED1" ]                                                    
        then                                                                     
               himm 0x111F0024 0x631	
               himm 0x12070000 3571
               himm 0x12070004 $3	
               himm 0x1207000C 0x5	
                                                    
        else                                                                         
                echo "open_wifi():should ap or sta!"                                 
        fi  
	himm 0x120D3100 0x40                                          
}  
                                                       
close_LED()                                                                         
{                                                                                    
	     		0                                    
        echo $1;                                          
        if [ "$1" = "LED0" ]                                
        then                       	
               himm 0x1207002C 0	
                               
        elif [ "$1" = "LED1" ]                                                    
        then                          
               himm 0x1207000C 0	
                                                    
        else                                                                         
                echo "open_wifi():should ap or sta!"                                 
        fi                                                                           
        himm 0x120D3100                                                                             
}                                                                                    
                                                                      
if [ $# != 3 ]                                                          
then                                                                                 
        echo "param error!"                                                          
else                                                                                 
        case $2 in                                                             
                open)                                                                  
                        open_LED $1 $3                                              
                ;;                                                                   
                close) close_LED $1 $3                                               
        esac                                                                         
fi               
