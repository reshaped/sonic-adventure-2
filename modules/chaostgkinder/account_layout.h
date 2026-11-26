/* Account dialog frames, labels, controls and their event handlers. */
#ifndef CHAOSTGKINDER_ACCOUNT_LAYOUT_H
#define CHAOSTGKINDER_ACCOUNT_LAYOUT_H
#include "chaostgkinder/account_profile_state.h"
#include "chaostgkinder/widget.h"

static const struct kinder_rect edit_rows[6] = {
#include "chaostgkinder/account_layout/edit_rows.inc"
};

static const struct kinder_rect signup_rows[6] = {
#include "chaostgkinder/account_layout/signup_rows.inc"
};

static const struct kinder_rect edit_focus[5] = {
#include "chaostgkinder/account_layout/edit_focus.inc"
};

static const struct kinder_rect signup_focus[5] = {
#include "chaostgkinder/account_layout/signup_focus.inc"
};

static const struct kinder_label_desc mail_address_style = {
#include "chaostgkinder/account_layout/mail_address_style.inc"
};

static const struct kinder_widget_desc mail_address = {
#include "chaostgkinder/account_layout/mail_address.inc"
};

static const struct kinder_scaled_label_desc mail_prompt_style = {
#include "chaostgkinder/account_layout/mail_prompt_style.inc"
};

static const struct kinder_widget_desc mail_prompt = {
#include "chaostgkinder/account_layout/mail_prompt.inc"
};

static const struct kinder_button_desc mail_yes_style = {
#include "chaostgkinder/account_layout/mail_yes_style.inc"
};

static kinder_notify mail_yes_input;
static const struct kinder_widget_desc mail_yes = {
#include "chaostgkinder/account_layout/mail_yes.inc"
};

static const struct kinder_button_desc mail_no_style = {
#include "chaostgkinder/account_layout/mail_no_style.inc"
};

static kinder_notify mail_no_input;
static const struct kinder_widget_desc mail_no = {
#include "chaostgkinder/account_layout/mail_no.inc"
};

static const struct kinder_texture_desc mail_dialog_fill_style = {
#include "chaostgkinder/account_layout/mail_dialog_fill_style.inc"
};

static const struct kinder_widget_desc mail_dialog_fill = {
#include "chaostgkinder/account_layout/mail_dialog_fill.inc"
};

static const struct kinder_rect mail_dialog_frame[8] = {
#include "chaostgkinder/account_layout/mail_dialog_frame.inc"
};

static const struct kinder_container_desc mail_dialog_style = {
#include "chaostgkinder/account_layout/mail_dialog_style.inc"
};

static kinder_notify mail_dialog_input;
static const struct kinder_widget_desc mail_dialog = {
#include "chaostgkinder/account_layout/mail_dialog.inc"
};

static const struct kinder_scaled_label_desc user_title_style = {
#include "chaostgkinder/account_layout/user_title_style.inc"
};

static const struct kinder_widget_desc user_title = {
#include "chaostgkinder/account_layout/user_title.inc"
};

static const struct kinder_scaled_label_desc user_prompt_style = {
#include "chaostgkinder/account_layout/user_prompt_style.inc"
};

static const struct kinder_widget_desc user_prompt = {
#include "chaostgkinder/account_layout/user_prompt.inc"
};

static const struct kinder_frame_desc user_field_style = {
#include "chaostgkinder/account_layout/user_field_style.inc"
};

static kinder_notify user_field_input;
static const struct kinder_widget_desc user_field = {
#include "chaostgkinder/account_layout/user_field.inc"
};

static const struct kinder_button_desc user_next_style = {
#include "chaostgkinder/account_layout/user_next_style.inc"
};

static kinder_notify user_next_input;
static const struct kinder_widget_desc user_next = {
#include "chaostgkinder/account_layout/user_next.inc"
};

static const struct kinder_button_desc user_quit_style = {
#include "chaostgkinder/account_layout/user_quit_style.inc"
};

static kinder_notify user_quit_input;
static const struct kinder_widget_desc user_quit = {
#include "chaostgkinder/account_layout/user_quit.inc"
};

static const struct kinder_texture_desc user_dialog_fill_style = {
#include "chaostgkinder/account_layout/user_dialog_fill_style.inc"
};

static const struct kinder_widget_desc user_dialog_fill = {
#include "chaostgkinder/account_layout/user_dialog_fill.inc"
};

static const struct kinder_rect user_dialog_frame[8] = {
#include "chaostgkinder/account_layout/user_dialog_frame.inc"
};

static const struct kinder_container_desc user_dialog_style = {
#include "chaostgkinder/account_layout/user_dialog_style.inc"
};

static kinder_notify user_dialog_input;
static const struct kinder_widget_desc user_dialog = {
#include "chaostgkinder/account_layout/user_dialog.inc"
};

static const struct kinder_label_desc profile_prompt_style = {
#include "chaostgkinder/account_layout/profile_prompt_style.inc"
};

static const struct kinder_widget_desc profile_prompt = {
#include "chaostgkinder/account_layout/profile_prompt.inc"
};

static const struct kinder_button_desc profile_next_style = {
#include "chaostgkinder/account_layout/profile_next_style.inc"
};

static kinder_notify profile_next_input;
static const struct kinder_widget_desc profile_next = {
#include "chaostgkinder/account_layout/profile_next.inc"
};

static const struct kinder_frame_desc profile_field_style = {
#include "chaostgkinder/account_layout/profile_field_style.inc"
};

static kinder_notify profile_field_input;
static const struct kinder_widget_desc profile_field = {
#include "chaostgkinder/account_layout/profile_field.inc"
};

static const struct kinder_texture_desc profile_dialog_fill_style = {
#include "chaostgkinder/account_layout/profile_dialog_fill_style.inc"
};

static const struct kinder_widget_desc profile_dialog_fill = {
#include "chaostgkinder/account_layout/profile_dialog_fill.inc"
};

static const struct kinder_rect profile_dialog_frame[8] = {
#include "chaostgkinder/account_layout/profile_dialog_frame.inc"
};

static const struct kinder_container_desc profile_dialog_style = {
#include "chaostgkinder/account_layout/profile_dialog_style.inc"
};

static kinder_notify profile_dialog_input;
static const struct kinder_widget_desc profile_dialog = {
#include "chaostgkinder/account_layout/profile_dialog.inc"
};

static const struct kinder_label_desc contact_prompt_style = {
#include "chaostgkinder/account_layout/contact_prompt_style.inc"
};

static const struct kinder_widget_desc contact_prompt = {
#include "chaostgkinder/account_layout/contact_prompt.inc"
};

static const struct kinder_frame_desc contact_field_style = {
#include "chaostgkinder/account_layout/contact_field_style.inc"
};

static kinder_notify contact_field_input;
static const struct kinder_widget_desc contact_field = {
#include "chaostgkinder/account_layout/contact_field.inc"
};

static const struct kinder_button_desc contact_next_style = {
#include "chaostgkinder/account_layout/contact_next_style.inc"
};

static kinder_notify contact_next_input;
static const struct kinder_widget_desc contact_next = {
#include "chaostgkinder/account_layout/contact_next.inc"
};

static const struct kinder_texture_desc contact_dialog_fill_style = {
#include "chaostgkinder/account_layout/contact_dialog_fill_style.inc"
};

static const struct kinder_widget_desc contact_dialog_fill = {
#include "chaostgkinder/account_layout/contact_dialog_fill.inc"
};

static const struct kinder_rect contact_dialog_frame[8] = {
#include "chaostgkinder/account_layout/contact_dialog_frame.inc"
};

static const struct kinder_container_desc contact_dialog_style = {
#include "chaostgkinder/account_layout/contact_dialog_style.inc"
};

static kinder_notify contact_dialog_input;
static const struct kinder_widget_desc contact_dialog = {
#include "chaostgkinder/account_layout/contact_dialog.inc"
};

static const struct kinder_label_desc visibility_prompt_style = {
#include "chaostgkinder/account_layout/visibility_prompt_style.inc"
};

static const struct kinder_widget_desc visibility_prompt = {
#include "chaostgkinder/account_layout/visibility_prompt.inc"
};

static const struct kinder_button_desc visibility_yes_style = {
#include "chaostgkinder/account_layout/visibility_yes_style.inc"
};

static kinder_notify visibility_yes_input;
static const struct kinder_widget_desc visibility_yes = {
#include "chaostgkinder/account_layout/visibility_yes.inc"
};

static const struct kinder_button_desc visibility_no_style = {
#include "chaostgkinder/account_layout/visibility_no_style.inc"
};

static kinder_notify visibility_no_input;
static const struct kinder_widget_desc visibility_no = {
#include "chaostgkinder/account_layout/visibility_no.inc"
};

static const struct kinder_texture_desc visibility_dialog_fill_style = {
#include "chaostgkinder/account_layout/visibility_dialog_fill_style.inc"
};

static const struct kinder_widget_desc visibility_dialog_fill = {
#include "chaostgkinder/account_layout/visibility_dialog_fill.inc"
};

static const struct kinder_rect visibility_dialog_frame[8] = {
#include "chaostgkinder/account_layout/visibility_dialog_frame.inc"
};

static const struct kinder_container_desc visibility_dialog_style = {
#include "chaostgkinder/account_layout/visibility_dialog_style.inc"
};

static kinder_notify visibility_dialog_input;
static const struct kinder_widget_desc visibility_dialog = {
#include "chaostgkinder/account_layout/visibility_dialog.inc"
};

static const struct kinder_scaled_label_desc location_prompt_style = {
#include "chaostgkinder/account_layout/location_prompt_style.inc"
};

static const struct kinder_widget_desc location_prompt = {
#include "chaostgkinder/account_layout/location_prompt.inc"
};

static const struct kinder_scaled_label_desc country_label_style = {
#include "chaostgkinder/account_layout/country_label_style.inc"
};

static const struct kinder_widget_desc country_label = {
#include "chaostgkinder/account_layout/country_label.inc"
};

static const struct kinder_scaled_label_desc region_label_style = {
#include "chaostgkinder/account_layout/region_label_style.inc"
};

static const struct kinder_widget_desc region_label = {
#include "chaostgkinder/account_layout/region_label.inc"
};

static kinder_notify country_choice_input;
static const struct kinder_widget_desc country_choice = {
#include "chaostgkinder/account_layout/country_choice.inc"
};

static kinder_notify district_choice_input;
static const struct kinder_widget_desc district_choice = {
#include "chaostgkinder/account_layout/district_choice.inc"
};

static kinder_notify region_choice_input;
static const struct kinder_widget_desc region_choice = {
#include "chaostgkinder/account_layout/region_choice.inc"
};

static const struct kinder_button_desc location_next_style = {
#include "chaostgkinder/account_layout/location_next_style.inc"
};

static kinder_notify location_next_input;
static const struct kinder_widget_desc location_next = {
#include "chaostgkinder/account_layout/location_next.inc"
};

static const struct kinder_texture_desc location_dialog_fill_style = {
#include "chaostgkinder/account_layout/location_dialog_fill_style.inc"
};

static const struct kinder_widget_desc location_dialog_fill = {
#include "chaostgkinder/account_layout/location_dialog_fill.inc"
};

static const struct kinder_rect location_dialog_frame[8] = {
#include "chaostgkinder/account_layout/location_dialog_frame.inc"
};

static const struct kinder_container_desc location_dialog_style = {
#include "chaostgkinder/account_layout/location_dialog_style.inc"
};

static kinder_notify location_dialog_input;
static const struct kinder_widget_desc location_dialog = {
#include "chaostgkinder/account_layout/location_dialog.inc"
};

static const struct kinder_scaled_label_desc submit_prompt_style = {
#include "chaostgkinder/account_layout/submit_prompt_style.inc"
};

static const struct kinder_widget_desc submit_prompt = {
#include "chaostgkinder/account_layout/submit_prompt.inc"
};

static const struct kinder_button_desc submit_yes_style = {
#include "chaostgkinder/account_layout/submit_yes_style.inc"
};

static kinder_notify submit_yes_input;
static const struct kinder_widget_desc submit_yes = {
#include "chaostgkinder/account_layout/submit_yes.inc"
};

static const struct kinder_button_desc submit_no_style = {
#include "chaostgkinder/account_layout/submit_no_style.inc"
};

static kinder_notify submit_no_input;
static const struct kinder_widget_desc submit_no = {
#include "chaostgkinder/account_layout/submit_no.inc"
};

static const struct kinder_texture_desc submit_dialog_fill_style = {
#include "chaostgkinder/account_layout/submit_dialog_fill_style.inc"
};

static const struct kinder_widget_desc submit_dialog_fill = {
#include "chaostgkinder/account_layout/submit_dialog_fill.inc"
};

static const struct kinder_rect submit_dialog_frame[8] = {
#include "chaostgkinder/account_layout/submit_dialog_frame.inc"
};

static const struct kinder_container_desc submit_dialog_style = {
#include "chaostgkinder/account_layout/submit_dialog_style.inc"
};

static kinder_notify submit_dialog_input;
static const struct kinder_widget_desc submit_dialog = {
#include "chaostgkinder/account_layout/submit_dialog.inc"
};

static const struct kinder_label_desc success_message_style = {
#include "chaostgkinder/account_layout/success_message_style.inc"
};

static const struct kinder_widget_desc success_message = {
#include "chaostgkinder/account_layout/success_message.inc"
};

static const struct kinder_texture_desc success_dialog_fill_style = {
#include "chaostgkinder/account_layout/success_dialog_fill_style.inc"
};

static const struct kinder_widget_desc success_dialog_fill = {
#include "chaostgkinder/account_layout/success_dialog_fill.inc"
};

static const struct kinder_rect success_dialog_frame[9] = {
#include "chaostgkinder/account_layout/success_dialog_frame.inc"
};

static const struct kinder_container_desc success_dialog_style = {
#include "chaostgkinder/account_layout/success_dialog_style.inc"
};

static kinder_notify success_dialog_input;
static const struct kinder_widget_desc success_dialog = {
#include "chaostgkinder/account_layout/success_dialog.inc"
};

static const struct kinder_label_desc taken_message_style = {
#include "chaostgkinder/account_layout/taken_message_style.inc"
};

static const struct kinder_widget_desc taken_message = {
#include "chaostgkinder/account_layout/taken_message.inc"
};

static const struct kinder_frame_desc taken_field_style = {
#include "chaostgkinder/account_layout/taken_field_style.inc"
};

static kinder_notify taken_field_input;
static const struct kinder_widget_desc taken_field = {
#include "chaostgkinder/account_layout/taken_field.inc"
};

static const struct kinder_button_desc taken_find_style = {
#include "chaostgkinder/account_layout/taken_find_style.inc"
};

static kinder_notify taken_find_input;
static const struct kinder_widget_desc taken_find = {
#include "chaostgkinder/account_layout/taken_find.inc"
};

static const struct kinder_button_desc taken_retry_style = {
#include "chaostgkinder/account_layout/taken_retry_style.inc"
};

static kinder_notify taken_retry_input;
static const struct kinder_widget_desc taken_retry = {
#include "chaostgkinder/account_layout/taken_retry.inc"
};

static const struct kinder_button_desc taken_remove_style = {
#include "chaostgkinder/account_layout/taken_remove_style.inc"
};

static kinder_notify taken_remove_input;
static const struct kinder_widget_desc taken_remove = {
#include "chaostgkinder/account_layout/taken_remove.inc"
};

static const struct kinder_texture_desc taken_dialog_fill_style = {
#include "chaostgkinder/account_layout/taken_dialog_fill_style.inc"
};

static const struct kinder_widget_desc taken_dialog_fill = {
#include "chaostgkinder/account_layout/taken_dialog_fill.inc"
};

static const struct kinder_rect taken_dialog_frame[8] = {
#include "chaostgkinder/account_layout/taken_dialog_frame.inc"
};

static const struct kinder_container_desc taken_dialog_style = {
#include "chaostgkinder/account_layout/taken_dialog_style.inc"
};

static kinder_notify taken_dialog_input;
static const struct kinder_widget_desc taken_dialog = {
#include "chaostgkinder/account_layout/taken_dialog.inc"
};

static const struct kinder_label_desc found_message_style = {
#include "chaostgkinder/account_layout/found_message_style.inc"
};

static const struct kinder_widget_desc found_message = {
#include "chaostgkinder/account_layout/found_message.inc"
};

static const struct kinder_frame_desc found_field_style = {
#include "chaostgkinder/account_layout/found_field_style.inc"
};

static kinder_notify found_field_input;
static const struct kinder_widget_desc found_field = {
#include "chaostgkinder/account_layout/found_field.inc"
};

static const struct kinder_button_desc found_find_style = {
#include "chaostgkinder/account_layout/found_find_style.inc"
};

static kinder_notify found_find_input;
static const struct kinder_widget_desc found_find = {
#include "chaostgkinder/account_layout/found_find.inc"
};

static const struct kinder_button_desc found_quit_style = {
#include "chaostgkinder/account_layout/found_quit_style.inc"
};

static kinder_notify found_quit_input;
static const struct kinder_widget_desc found_quit = {
#include "chaostgkinder/account_layout/found_quit.inc"
};

static const struct kinder_button_desc found_remove_style = {
#include "chaostgkinder/account_layout/found_remove_style.inc"
};

static kinder_notify found_remove_input;
static const struct kinder_widget_desc found_remove = {
#include "chaostgkinder/account_layout/found_remove.inc"
};

static const struct kinder_texture_desc found_dialog_fill_style = {
#include "chaostgkinder/account_layout/found_dialog_fill_style.inc"
};

static const struct kinder_widget_desc found_dialog_fill = {
#include "chaostgkinder/account_layout/found_dialog_fill.inc"
};

static const struct kinder_rect found_dialog_frame[8] = {
#include "chaostgkinder/account_layout/found_dialog_frame.inc"
};

static const struct kinder_container_desc found_dialog_style = {
#include "chaostgkinder/account_layout/found_dialog_style.inc"
};

static kinder_notify found_dialog_input;
static const struct kinder_widget_desc found_dialog = {
#include "chaostgkinder/account_layout/found_dialog.inc"
};

static const struct kinder_caption_desc missing_title_style = {
#include "chaostgkinder/account_layout/missing_title_style.inc"
};

static const struct kinder_widget_desc missing_title = {
#include "chaostgkinder/account_layout/missing_title.inc"
};

static const struct kinder_caption_desc missing_message_style = {
#include "chaostgkinder/account_layout/missing_message_style.inc"
};

static const struct kinder_widget_desc missing_message = {
#include "chaostgkinder/account_layout/missing_message.inc"
};

static const struct kinder_texture_desc missing_dialog_fill_style = {
#include "chaostgkinder/account_layout/missing_dialog_fill_style.inc"
};

static const struct kinder_widget_desc missing_dialog_fill = {
#include "chaostgkinder/account_layout/missing_dialog_fill.inc"
};

static const struct kinder_rect missing_dialog_frame[9] = {
#include "chaostgkinder/account_layout/missing_dialog_frame.inc"
};

static const struct kinder_container_desc missing_dialog_style = {
#include "chaostgkinder/account_layout/missing_dialog_style.inc"
};

static kinder_notify missing_dialog_input;
static const struct kinder_widget_desc missing_dialog = {
#include "chaostgkinder/account_layout/missing_dialog.inc"
};

static const struct kinder_label_desc remove_message_style = {
#include "chaostgkinder/account_layout/remove_message_style.inc"
};

static const struct kinder_widget_desc remove_message = {
#include "chaostgkinder/account_layout/remove_message.inc"
};

static const struct kinder_button_desc remove_yes_style = {
#include "chaostgkinder/account_layout/remove_yes_style.inc"
};

static kinder_notify remove_yes_input;
static const struct kinder_widget_desc remove_yes = {
#include "chaostgkinder/account_layout/remove_yes.inc"
};

static const struct kinder_button_desc remove_no_style = {
#include "chaostgkinder/account_layout/remove_no_style.inc"
};

static kinder_notify remove_no_input;
static const struct kinder_widget_desc remove_no = {
#include "chaostgkinder/account_layout/remove_no.inc"
};

static const struct kinder_texture_desc remove_dialog_fill_style = {
#include "chaostgkinder/account_layout/remove_dialog_fill_style.inc"
};

static const struct kinder_widget_desc remove_dialog_fill = {
#include "chaostgkinder/account_layout/remove_dialog_fill.inc"
};

static const struct kinder_rect remove_dialog_frame[8] = {
#include "chaostgkinder/account_layout/remove_dialog_frame.inc"
};

static const struct kinder_container_desc remove_dialog_style = {
#include "chaostgkinder/account_layout/remove_dialog_style.inc"
};

static kinder_notify remove_dialog_input;
static const struct kinder_widget_desc remove_dialog = {
#include "chaostgkinder/account_layout/remove_dialog.inc"
};

static const struct kinder_label_desc removed_message_style = {
#include "chaostgkinder/account_layout/removed_message_style.inc"
};

static const struct kinder_widget_desc removed_message = {
#include "chaostgkinder/account_layout/removed_message.inc"
};

static const struct kinder_texture_desc removed_dialog_fill_style = {
#include "chaostgkinder/account_layout/removed_dialog_fill_style.inc"
};

static const struct kinder_widget_desc removed_dialog_fill = {
#include "chaostgkinder/account_layout/removed_dialog_fill.inc"
};

static const struct kinder_rect removed_dialog_frame[9] = {
#include "chaostgkinder/account_layout/removed_dialog_frame.inc"
};

static const struct kinder_container_desc removed_dialog_style = {
#include "chaostgkinder/account_layout/removed_dialog_style.inc"
};

static kinder_notify removed_dialog_input;
static const struct kinder_widget_desc removed_dialog = {
#include "chaostgkinder/account_layout/removed_dialog.inc"
};

static const struct kinder_caption_desc edit_user_label_style = {
#include "chaostgkinder/account_layout/edit_user_label_style.inc"
};

static const struct kinder_widget_desc edit_user_label = {
#include "chaostgkinder/account_layout/edit_user_label.inc"
};

static const struct kinder_caption_desc edit_profile_label_style = {
#include "chaostgkinder/account_layout/edit_profile_label_style.inc"
};

static const struct kinder_widget_desc edit_profile_label = {
#include "chaostgkinder/account_layout/edit_profile_label.inc"
};

static const struct kinder_caption_desc edit_country_label_style = {
#include "chaostgkinder/account_layout/edit_country_label_style.inc"
};

static const struct kinder_widget_desc edit_country_label = {
#include "chaostgkinder/account_layout/edit_country_label.inc"
};

static const struct kinder_caption_desc edit_region_label_style = {
#include "chaostgkinder/account_layout/edit_region_label_style.inc"
};

static const struct kinder_widget_desc edit_region_label = {
#include "chaostgkinder/account_layout/edit_region_label.inc"
};

static const struct kinder_caption_desc edit_contact_label_style = {
#include "chaostgkinder/account_layout/edit_contact_label_style.inc"
};

static const struct kinder_widget_desc edit_contact_label = {
#include "chaostgkinder/account_layout/edit_contact_label.inc"
};

static const struct kinder_caption_desc edit_visibility_label_style = {
#include "chaostgkinder/account_layout/edit_visibility_label_style.inc"
};

static const struct kinder_widget_desc edit_visibility_label = {
#include "chaostgkinder/account_layout/edit_visibility_label.inc"
};

static const struct kinder_caption_desc edit_user_name_style = {
#include "chaostgkinder/account_layout/edit_user_name_style.inc"
};

static const struct kinder_widget_desc edit_user_name = {
#include "chaostgkinder/account_layout/edit_user_name.inc"
};

static const struct kinder_frame_desc edit_profile_field_style = {
#include "chaostgkinder/account_layout/edit_profile_field_style.inc"
};

static kinder_notify edit_profile_field_input;
static const struct kinder_widget_desc edit_profile_field = {
#include "chaostgkinder/account_layout/edit_profile_field.inc"
};

static kinder_notify edit_country_input;
static const struct kinder_widget_desc edit_country = {
#include "chaostgkinder/account_layout/edit_country.inc"
};

static kinder_notify edit_region_input;
static const struct kinder_widget_desc edit_region = {
#include "chaostgkinder/account_layout/edit_region.inc"
};

static kinder_notify edit_district_input;
static const struct kinder_widget_desc edit_district = {
#include "chaostgkinder/account_layout/edit_district.inc"
};

static const struct kinder_frame_desc edit_contact_field_style = {
#include "chaostgkinder/account_layout/edit_contact_field_style.inc"
};

static kinder_notify edit_contact_field_input;
static const struct kinder_widget_desc edit_contact_field = {
#include "chaostgkinder/account_layout/edit_contact_field.inc"
};

static const struct kinder_list_item_desc visibility_options[2] = {
#include "chaostgkinder/account_layout/visibility_options.inc"
};

static const struct kinder_list_desc edit_visibility_style = {
#include "chaostgkinder/account_layout/edit_visibility_style.inc"
};

static kinder_notify edit_visibility_input;
static const struct kinder_widget_desc edit_visibility = {
#include "chaostgkinder/account_layout/edit_visibility.inc"
};

static const struct kinder_button_desc edit_submit_style = {
#include "chaostgkinder/account_layout/edit_submit_style.inc"
};

static kinder_notify edit_submit_input;
static const struct kinder_widget_desc edit_submit = {
#include "chaostgkinder/account_layout/edit_submit.inc"
};

static const struct kinder_texture_desc edit_dialog_fill_style = {
#include "chaostgkinder/account_layout/edit_dialog_fill_style.inc"
};

static const struct kinder_widget_desc edit_dialog_fill = {
#include "chaostgkinder/account_layout/edit_dialog_fill.inc"
};

static const struct kinder_rect edit_dialog_frame[8] = {
#include "chaostgkinder/account_layout/edit_dialog_frame.inc"
};

static const struct kinder_container_desc edit_dialog_style = {
#include "chaostgkinder/account_layout/edit_dialog_style.inc"
};

static kinder_notify edit_dialog_input;
static const struct kinder_widget_desc edit_dialog = {
#include "chaostgkinder/account_layout/edit_dialog.inc"
};

static const struct kinder_label_desc isp_message_style = {
#include "chaostgkinder/account_layout/isp_message_style.inc"
};

static const struct kinder_widget_desc isp_message = {
#include "chaostgkinder/account_layout/isp_message.inc"
};

static const struct kinder_texture_desc isp_dialog_fill_style = {
#include "chaostgkinder/account_layout/isp_dialog_fill_style.inc"
};

static const struct kinder_widget_desc isp_dialog_fill = {
#include "chaostgkinder/account_layout/isp_dialog_fill.inc"
};

static const struct kinder_rect isp_dialog_frame[9] = {
#include "chaostgkinder/account_layout/isp_dialog_frame.inc"
};

static const struct kinder_container_desc isp_dialog_style = {
#include "chaostgkinder/account_layout/isp_dialog_style.inc"
};

static kinder_notify isp_dialog_input;
static const struct kinder_widget_desc isp_dialog = {
#include "chaostgkinder/account_layout/isp_dialog.inc"
};

static const struct kinder_caption_desc disconnect_message_style = {
#include "chaostgkinder/account_layout/disconnect_message_style.inc"
};

static const struct kinder_widget_desc disconnect_message = {
#include "chaostgkinder/account_layout/disconnect_message.inc"
};

static const struct kinder_texture_desc disconnect_dialog_fill_style = {
#include "chaostgkinder/account_layout/disconnect_dialog_fill_style.inc"
};

static const struct kinder_widget_desc disconnect_dialog_fill = {
#include "chaostgkinder/account_layout/disconnect_dialog_fill.inc"
};

static const struct kinder_rect disconnect_dialog_frame[9] = {
#include "chaostgkinder/account_layout/disconnect_dialog_frame.inc"
};

static const struct kinder_container_desc disconnect_dialog_style = {
#include "chaostgkinder/account_layout/disconnect_dialog_style.inc"
};

static kinder_notify disconnect_dialog_input;
static const struct kinder_widget_desc disconnect_dialog = {
#include "chaostgkinder/account_layout/disconnect_dialog.inc"
};

static const struct kinder_caption_desc failure_message_style = {
#include "chaostgkinder/account_layout/failure_message_style.inc"
};

static const struct kinder_widget_desc failure_message = {
#include "chaostgkinder/account_layout/failure_message.inc"
};

static const struct kinder_texture_desc failure_dialog_fill_style = {
#include "chaostgkinder/account_layout/failure_dialog_fill_style.inc"
};

static const struct kinder_widget_desc failure_dialog_fill = {
#include "chaostgkinder/account_layout/failure_dialog_fill.inc"
};

static const struct kinder_rect failure_dialog_frame[9] = {
#include "chaostgkinder/account_layout/failure_dialog_frame.inc"
};

static const struct kinder_container_desc failure_dialog_style = {
#include "chaostgkinder/account_layout/failure_dialog_style.inc"
};

static kinder_notify failure_dialog_input;
static const struct kinder_widget_desc failure_dialog = {
#include "chaostgkinder/account_layout/failure_dialog.inc"
};

#endif
