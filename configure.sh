#! /bin/bash

SERVICE=Fitting_Room_Video_Looper.service

systemctl disable $SERVICE

cp Fitting_Room_Video_Looper.service /etc/systemd/system/

systemctl daemon-reload
systemctl enable $SERVICE

echo "Configuration completed"
