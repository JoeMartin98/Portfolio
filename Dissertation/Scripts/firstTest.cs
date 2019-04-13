using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

namespace UnityStandardAssets.Vehicles.Car
{
    public class firstTest : MonoBehaviour
    {
        //DECLARE PUBLIC VARIABLES
        public GameObject warning;
        public GameObject button;
        public GameObject backButton;
        public GameObject failed;
        public Text reactTime;
        public Text reactionTime;
        public Text speedText;

        //DECLARE PRIVATE VARIABLES
        private CarController carInfo;
        private float maxTime = 20.0f;
        private float minTime = 4.0f;
        private float currentTime;
        private float fReactTime;
        private float startTime;
        private bool warned = false;
        private bool finished = false;
        private bool notified = false;
        private bool reactiveShown = false;

       public void Start()
        {
            warning.SetActive(false);
            SetRandomTime();
            fReactTime = 0;
            currentTime = minTime;
            carInfo = FindObjectOfType<CarController>();
        }

        void FixedUpdate()
        {
            speedText.text = carInfo.CurrentSpeed.ToString("#.0") + " mph";

            if (!finished) currentTime += Time.deltaTime;

            if ((currentTime >= startTime) && !warned)
            {
                if (carInfo.CurrentSpeed < 29.0f)
                {
                    reactionTime.text = "FAIL : Didn't reach 30mph!";
                    finished = true;
                }
                else
                {
                    startWarning();
                    warned = true;
                }
            }

            if (Input.GetKeyDown("s"))
            {
                if (warned && !reactiveShown)
                {
                    fReactTime += currentTime;
                    reactiveShown = true;
                }
                else
                {
                    finished = true;
                    failed.SetActive(true);
                }
            }

            if (carInfo.CurrentSpeed <= 1.0f && warned && !notified)
            {
                notified = true;
                reactTime.text = "Time to Stop : ";
                reactTime.text += currentTime.ToString("#.000") + " Seconds";
                reactionTime.text = "Reaction time : ";
                if (fReactTime == 0.00f) reactionTime.text += "ERROR! Please try again...";
                else reactionTime.text += fReactTime.ToString("#.000") + " Seconds";
                warning.SetActive(false);
                finished = true;
            }

            if (finished)
            {
                carInfo.speed = 0;
                carInfo.m_Rigidbody.velocity = new Vector3(0, 0, 0);
                button.SetActive(true);
                backButton.SetActive(true);
            }
        }

        void startWarning()
        {
            warning.SetActive(true);
            currentTime = 0;
        }

        void SetRandomTime()
        {
            startTime = Random.Range(minTime, maxTime);
        }

        public void repeat()
        {
            SceneManager.LoadScene("SampleScene");
        }

        public void stop()
        {
            SceneManager.LoadScene("menu");
        }
    }
}