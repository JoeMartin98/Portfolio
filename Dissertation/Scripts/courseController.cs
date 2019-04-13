using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace UnityStandardAssets.Vehicles.Car
{
    public class courseController : MonoBehaviour
    {
        public GameObject player;
        public GameObject button1;
        public GameObject button2;
        public GameObject crash;
        public Text speedText;
        public Text timeText;
        public Text averageText;
        public Text overText;
        public GameObject warning;
        public Text reactTime;
        public Text reactionTime;
        public bool finished = false;
        public bool crashed = false;

        private CarController carInfo;
        private float currentTime;
        private float currentTimeReaction;       
        private int speed;
        private float speedCount;
        private float averageSpeed;
        private float overSpeed;
        private int count;
        private float maxTime = 30.0f;
        private float minTime = 5.0f;
        private float startTime;
        private float fReactTime;
        private bool warned = false;
        private bool notified = false;
        private bool reactiveShown = false;
        private bool reactionFinished = false;

        void Start()
        {
            carInfo = FindObjectOfType<CarController>();
            PlayerPrefs.SetInt("speed", 40);
            count = 0;
            overSpeed = 0.0f;
            currentTimeReaction = minTime;
            startTime = Random.Range(minTime, maxTime);
        }

        void FixedUpdate()
        {
            Calculate();
            EmergencyStop();
        }

        void Calculate()
        {
            if (carInfo.crashed) finished = true;
            if (!finished)
            {
                speedText.text = carInfo.CurrentSpeed.ToString("#.0") + " mph";
                currentTime += Time.deltaTime;
                if(!reactionFinished) currentTimeReaction += Time.deltaTime;
                timeText.text = currentTime.ToString("#.00") + " seconds";
                count++;
                speedCount += carInfo.CurrentSpeed;
            }
            else
            {
                //COMPLETE
                if (carInfo.crashed == false)
                {            
                    averageSpeed = speedCount / count;
                    averageText.text = "Average speed = " + averageSpeed.ToString("#.00") + "mph";
                    overText.text = "Time over the speed limit = " + overSpeed.ToString("#.00") + " seconds";
                    reactTime.text = "Time to Stop : ";
                    reactTime.text += currentTimeReaction.ToString("#.000") + " Seconds";
                    reactionTime.text = "Reaction time : ";
                    if (fReactTime == 0.00f) reactionTime.text += "ERROR! Please try again...";
                    else reactionTime.text += fReactTime.ToString("#.000") + " Seconds";
                }
                else crash.SetActive(true);

                carInfo.speed = 0;
                carInfo.m_Rigidbody.velocity = new Vector3(0, 0, 0);
                button1.SetActive(true);
                button2.SetActive(true);
            }

            speed = PlayerPrefs.GetInt("speed");
            if (carInfo.CurrentSpeed > speed + 2) overSpeed += Time.deltaTime;
        }

        void EmergencyStop()
        {
            if ((currentTimeReaction >= startTime) && !warned)
            {
                startWarning();
                warned = true;
            }

            if (Input.GetKeyDown("s"))
            {
                if (warned && !reactiveShown)
                {
                    fReactTime += currentTimeReaction;
                    reactiveShown = true;
                }
            }

            if (carInfo.CurrentSpeed <= 1.0f && warned && !notified)
            {
                notified = true;
                reactionFinished = true;
                warning.SetActive(false);
            }
        }

        void OnTriggerEnter(Collider other)
        {
            //PLAYER IS AT THE END          
            finished = true;
            crashed = false;
        }

        void startWarning()
        {
            warning.SetActive(true);
            currentTimeReaction = 0;
        }
    }
}