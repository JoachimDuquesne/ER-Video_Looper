
systemctl --system disable Fitting_Room_Video_Looper.service


systemctl stop Fitting_Room_Video_Looper.service
systemctl delete Fitting_Room_Video_Looper.service

systemctl daemon-reload

rm -f /etc/systemd/system/Fitting_Room_Video_Looper.service
