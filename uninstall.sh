systemctl --global disable Escape_Rush_Entrance_Room_Video_Looper.service
systemctl --system disable Escape_Rush_Entrance_Room_Video_Looper.service
systemctl --user disable Escape_Rush_Entrance_Room_Video_Looper.service

systemctl stop Escape_Rush_Entrance_Room_Video_Looper.service
systemctl delete Escape_Rush_Entrance_Room_Video_Looper.service

systemctl daemon-reload

rm -f /etc/systemd/system/Escape_Rush_Entrance_Room_Video_Looper.service
rm -f /etc/systemd/global/Escape_Rush_Entrance_Room_Video_Looper.service
rm -f /etc/systemd/user/Escape_Rush_Entrance_Room_Video_Looper.service
