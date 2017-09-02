
SERVICE=Fitting_Room_Video_Looper.service



systemctl --system disable $SERVICE
systemctl stop $SERVICE
systemctl delete $SERVICE

systemctl daemon-reload

rm -f /etc/systemd/system/$SERVICE
