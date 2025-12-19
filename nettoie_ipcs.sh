#!/bin/bash

echo "Suppression des segments de mémoire partagée..."
for id in $(ipcs -m | awk 'NR>3 {print $2}'); do
    ipcrm -m "$id"
done

echo "Suppression des sémaphores..."
for id in $(ipcs -s | awk 'NR>3 {print $2}'); do
    ipcrm -s "$id"
done

echo "Suppression des files de messages..."
for id in $(ipcs -q | awk 'NR>3 {print $2}'); do
    ipcrm -q "$id"
done

echo "Tous les IPC ont été supprimés."