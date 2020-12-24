#!/bin/sh

#resource data addr align*/
ALIGN=16
#resource max count*/
PDT_RES_MAX_CNT=5
#resource magic start*/
PDT_RES_MAGIC_START=0x72657373
#resource magic end*/
PDT_RES_MAGIC_END=0x72657364

function Usage()
{
	echo ""
	echo "Usage: $0 [resRootDir] [resList] [resImage]"
	echo ""
	echo "resRootDir:  resource file root path"
	echo ""
	echo "resList:  a text file containing the purpose and path of resources"
	echo "purpose:  1--boot logo"
	echo "          2--boot sound"
	echo "          3--OSD font library"
	echo "          4--OSD logo(main stream)"
	echo "          5--OSD logo(sub stream)"
	echo "path:     relative path relative to the resRootDir"
	echo ""
	echo "for example:"
	echo -e "\e[1;35mpurpose:1 path:./bootlogo.yuv\e[0m"
	echo -e "\e[1;35mpurpose:2 path:./bootsound.pcm\e[0m"
	echo -e "\e[1;35m... ...\e[0m"
	echo ""
	echo "resImage:  generated resource image file Path"
	echo ""
}

function ParseResList()
{
	dos2unix $1
	resCnt=0
	local i=1
	while read line
	do
		if [ ${resCnt} -gt ${PDT_RES_MAX_CNT} ];then
			echo "error: too many resources!"
			exit -1
		fi
		#echo $line
		resPurpose[$i]=$(awk 'BEGIN{printf("%08x",'${line: 8 :1}')}')
		resPath[$i]=${resRootDir}/${line#*path:}
		resCnt=$i
		echo "purpose=${resPurpose[$i]} path=${resPath[$i]}"
		((i++))
	done < $1
	#echo "resCnt=${resCnt}"
	if [ ${resCnt} -lt 1 ];then
		echo "error: too few resources!"
		exit -1
	fi
}

function ParseArgs()
{
	local argc=$#
	if [ ${argc} -ne 3 ];then
		Usage
		exit -1
	fi
	resRootDir=$1
	ParseResList $2
	imagePath=$3
}

function MakeImageData()
{
	imageData=${imagePath}_Data
	cat /dev/null > ${imageData}
	local i=1
	while [ $i -le ${PDT_RES_MAX_CNT} ]
	do
		if [  -e "${resPath[$i]}" ];then
			#resource length aligned by ALIGN
			local blockCnt=$(awk 'BEGIN{printf("%d",('$(stat -c "%s" ${resPath[$i]})'+'${ALIGN}'-1)/'${ALIGN}')}')
			resOffset[$i]=$(awk 'BEGIN{printf("%08x",'$(stat -c "%s" ${imageData})')}')
			#echo "dd if=${resPath[$i]} count=${blockCnt} bs=${ALIGN} conv=sync >> ${imageData}"
			dd if=${resPath[$i]} count=${blockCnt} bs=${ALIGN} conv=sync >> ${imageData}
		fi
		((i++))
	done
}

function MakeImageHead()
{
	imageHead=${imagePath}_Head
	imageHeadLen=$(awk 'BEGIN{printf("%d",(4*2+'${PDT_RES_MAX_CNT}'*4*3))}')
	#image head length aligned by ALIGN
	imageHeadBlockCnt=$(awk 'BEGIN{printf("%d",('${imageHeadLen}'+'${ALIGN}'-1)/'${ALIGN}')}')
	imageDataBaseOffset=$(awk 'BEGIN{printf("%08x",'${imageHeadBlockCnt}'*'${ALIGN}')}')
	echo -n ${PDT_RES_MAGIC_START} | xxd -r -ps > ${imageHead}
	local i=1
	while [ $i -le ${PDT_RES_MAX_CNT} ]
	do
		if [ ! -e "${resPath[$i]}" ];then
			echo "resPath[$i]=${resPath[$i]}: No such file"
			resPurpose[$i]=$(awk 'BEGIN{printf("%08x",0)}')
			resSize[$i]=$(awk 'BEGIN{printf("%08x",0)}')
			resOffset[$i]=$(awk 'BEGIN{printf("%08x",0)}')
		else
			resSize[$i]=$(awk 'BEGIN{printf("%08x",'$(stat -c "%s" ${resPath[$i]})')}')
			resOffset[$i]=$(awk 'BEGIN{printf("%08x",0x'${imageDataBaseOffset}'+0x'${resOffset[$i]}')}')
		fi
		#echo "resPurpose[$i]=${resPurpose[$i]}"
		#echo "resSize[$i]=${resSize[$i]}"
		#echo "resOffset[$i]=${resOffset[$i]}"
		echo -n ${resPurpose[$i]} | xxd -r -ps >> ${imageHead}
		echo -n ${resSize[$i]} | xxd -r -ps >> ${imageHead}
		echo -n ${resOffset[$i]} | xxd -r -ps >> ${imageHead}
		((i++))
	done
	echo -n ${PDT_RES_MAGIC_END} | xxd -r -ps >> ${imageHead}
}

function MakeImage()
{
	MakeImageData
	MakeImageHead
	dd if=${imageHead} of=${imagePath} count=${imageHeadBlockCnt} bs=${ALIGN} conv=sync
	dd if=${imageData} >> ${imagePath}
	rm ${imageHead}
	rm ${imageData}
}

ParseArgs $*
MakeImage