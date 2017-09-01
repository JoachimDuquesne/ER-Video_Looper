#! /bin/bash


PROGRAM_DIR=/home/EscapeRush/Fitting_Room


cd /etc/systemd/system

# Clear the file
echo "" >  Fitting_Room_Video_Looper.service

# Write the file
echo "[Unit]"                                             >> Fitting_Room_Video_Looper.service
echo "Description=Fitting-room video looper="             >> Fitting_Room_Video_Looper.service
#echo "After=multi-user.target"                           >> Fitting_Room_Video_Looper.service
echo ""                                                   >> Fitting_Room_Video_Looper.service
echo "[Service]"                                          >> Fitting_Room_Video_Looper.service
echo "Type=idle"                                          >> Fitting_Room_Video_Looper.service
echo "ExecStart=$PROGRAM_DIR/FittingRoom"                 >> Fitting_Room_Video_Looper.service
echo ""                                                   >> Fitting_Room_Video_Looper.service
echo "[Install]"                                          >> Fitting_Room_Video_Looper.service
echo "WantedBy=multi-user.target"                         >> Fitting_Room_Video_Looper.service

systemctl daemon-reload
systemctl enable Fitting_Room_Video_Looper.service

echo "Configuration completed"
