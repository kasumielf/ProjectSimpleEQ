using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using Objects;

public class BaseEnemy : MonoBehaviour {
    public TextMesh name;
    Objects.Object baseObject;

	// Use this for initialization
	void Start () {
        name.text = baseObject.name;
	}
	
	// Update is called once per frame
	void Update () {
        transform.position = new Vector3(this.baseObject.x * 4, 0.0f, this.baseObject.y * 4);
	}
}
