using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace UnityStandardAssets.Vehicles.Car
{
    public class SpeedController : MonoBehaviour
    {
        public int speed;

        void OnTriggerEnter(Collider other)
        {
            //PLAYER CHANGES SPEED
            PlayerPrefs.SetInt("speed", speed);
        }
    }
}