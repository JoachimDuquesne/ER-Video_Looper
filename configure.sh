#! /bin/bash


PROGRAM_DIR=/home//Video_Looper
PROGRAM=Fitting_Room
SERVICE=Fitting_Room_Video_Looper.service


cd /etc/systemd/system

# Clear the file
echo "" >  $SERVICE

# Write the file
echo "[Unit]"                                             >> $SERVICE
echo "Description=Fitting-room video looper"              >> $SERVICE
#echo "After=multi-user.target"                           >> $SERVICE
echo ""                                                   >> $SERVICE
echo "[Service]"                                          >> $SERVICE
echo "Type=idle"                                          >> $SERVICE
echo "ExecStart=$PROGRAM_DIR/$PROGRAM"                    >> $SERVICE
echo "User=pi"						  >> $SERVICE
echo "Group=pi"						  >> $SERVICE
echo ""                                                   >> $SERVICE
echo "[Install]"                                          >> $SERVICE
echo "WantedBy=multi-user.target"                         >> $SERVICE

systemctl daemon-reload
systemctl enable $SERVICE

echo "Configuration completed"
