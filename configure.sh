#! /bin/bash


PROGRAM_DIR=/home/EscapeRush/Entrance_Room


cd /etc/systemd/system

# Clear the file
echo "" >  Escape_Rush_Entrance_Room_Video_Looper.service

# Write the file
echo "[Unit]"                                             >> Escape_Rush_Entrance_Room_Video_Looper.service
echo "Description=Entrance-room video looper="            >> Escape_Rush_Entrance_Room_Video_Looper.service
#echo "After=multi-user.target"                           >> Escape_Rush_Entrance_Room_Video_Looper.service
echo ""                                                   >> Escape_Rush_Entrance_Room_Video_Looper.service
echo "[Service]"                                          >> Escape_Rush_Entrance_Room_Video_Looper.service
echo "Type=idle"                                          >> Escape_Rush_Entrance_Room_Video_Looper.service
echo "ExecStart=$PROGRAM_DIR/Entrance-room-video-looper"  >> Escape_Rush_Entrance_Room_Video_Looper.service
echo ""                                                   >> Escape_Rush_Entrance_Room_Video_Looper.service
echo "[Install]"                                          >> Escape_Rush_Entrance_Room_Video_Looper.service
echo "WantedBy=multi-user.target"                         >> Escape_Rush_Entrance_Room_Video_Looper.service

systemctl daemon-reload
systemctl enable Escape_Rush_Entrance_Room_Video_Looper.service

echo "Configuration completed"
