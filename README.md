#ResponsiveMOOC
*Making Massively Online Open Courseware responsive to viewers by making second-by-second, focus-sensitive modifications to video content. Presented by Raymond Chen, Devyash Lodha, Stephen Mylabathula, and Viet Tran.*


##Synposis
ResponsiveMOOC is a new approach to making existing digital educational content responsive to the user's level of focus and concentration. By leveraging signals from Emotiv and pupil tracking technologies, we are able to heuristically assess a viewer's attention level, and adapt video content dynamically appropriately: if the user's pupils are dilated [2] and Emotiv indicates the viewer is attentive, the video playback speed will increase to take advantage of a high rate of knowledge intake in the user. On the other hand, if signals indicate that the content is losing the user's attention, the video will be "intensified" (think brightness, hue saturation, contrast, and volume) in hopes of recapturing our user's attention.

##Technologies Used
**Emotiv**:

**Node.js**:

##System Design
ResponsiveMOOC consists of two primary components:

**Signal Capture**: The signal capture layer takes inputs from Emotiv and eye trackers in order to assess the viewer's current level of focus on the content at hand. That focus is transmitted to the video manipulation module, which modifies the content accordingly.

**Video Manipulation**: The video manipulation module modifies video to make it more eye-catching in the event that our system detects a decline in user attention, and increases the playback speed when a it detects opportuniites to increase bitrate to the viewer.


##Applications
Improving video content on the fly has the potential to bridge the gap between MOOC and traditional forms of education by paving the way for microadjustmens in content to compensate for the organic changes in style and expressiveness live human instructors employ to keep their audience engaged. Instead of having a face to subconsciously mimic, viewers [1] can now enjoy instruction that is responsive in its own right, and with time the signals and responses can be finely tuned to optimize knowledge intake and retention on a moment-by-moment basis. Early clients may include enterprise educational courses, where some companies have been more progressive in adopting wholly digital courseware, while also being deeply invested in improving the attentiveness of the user during a course.

##Next Steps
Given the opportunity to continue working on ResponsiveMOOC, some next steps might include
- **More Signals**: Currently we are only assessing focus with a scalar, but there are many low-hanging opportunities to improve the resolution of signal we use. For one, purchasing the premium SDK for Emotiv will unlock much richer information about the user (namely the raw data), rather than just a single numerical summary of a user's focus level.
- **More Manipulations**: Currently we modify only brightness and playback speed. By toying with hue saturation, image contrast, gamma, among other quick-to-manipulate image properties, we can potentially improve the video manipulation module's success in recapturing viewer attention.


##Credits
- Proudly implemented by **Raymond Chen, Devyash Lodha  Stephen Mylabathula**, and **Viet Tran**
- Special thanks to former teammate **Gary Tong** and our mentor **Yuan Wang** for helping us form our team.
- Special thanks to our mentor **Yuan Wang** for the idea, research and mentorship.

##References
1. Weatherholtz K, Campbell-Kibler K, Jaeger TF. Socially-Mediated Syntactic Alignment. Language Variation and Change. 2015.

2. Kang Olivia E., Huffer, Wheatley. Pupil Dilation Dynamics Track Attention to High-Level Information. The Public Library of Science (PLOS). 2014.
