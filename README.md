
Vacation Tracker is used to tracking employee vacation days in small and medium sized businesses.  The Daemon is used in combination with [Vacation Tracker Client](https://github.com/jbagg/vactracker)

###Features

* Managers get email notifications when their employee makes a vacation request.
* Employees get email notifications when their manager approves or rejects their vacation request.
* There are four types of employees: Normal, Manager, Admin (HR) and Disabled.
* The vacation request length calculation takes into account any statutory holidays during the vacation period.
* Multiple regions can be specified for business that operate in multiple provinces / states.
* Statutory holidays can be entered by region.
* An employees is assigned a region.
* Annual earned vacation is added to the employees balance on Jan 1st of each year.
* Employees have an individual annual earned vacation amount editable by admin / HR users.
* The Client GUI uses ZeroConf to discover the server / daemon, so no configuration is necessary on the employees desktops.
* Cross platform - works on Windows, Mac, Linux and Android.

###Language

Vacation Tracker Daemon is written in C++ using the Qt tool kit.