/*
 * Copyright 2016, Dario Casalinuovo. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include <SimpleMediaClient.h>
#include <MediaConnection.h>
#include <SupportDefs.h>

#include <debug.h>

#include <assert.h>
#include <stdio.h>

#ifdef DEBUG
#define DELAYED_MODE 1
#define SNOOZE_FOR 10000000
#endif


static BSimpleMediaClient* sProducer = NULL;
static BSimpleMediaClient* sConsumer = NULL;
static BSimpleMediaClient* sFilter = NULL;


void _InitClients(bool hasFilter)
{
	sProducer = new BSimpleMediaClient("MediaClientProducer");
	sConsumer = new BSimpleMediaClient("MediaClientConsumer");

	if (hasFilter)
		sFilter = new BSimpleMediaClient("MediaClientFilter");
	else
		sFilter = NULL;
}


void _DeleteClients()
{
	delete sProducer;
	delete sConsumer;
	delete sFilter;
}


media_format _BuildRawAudioFormat()
{
	media_format format;
	format.type = B_MEDIA_RAW_AUDIO;
	format.u.raw_audio = media_multi_audio_format::wildcard;

	return format;
}


void _ConsumerProducerTest()
{
	_InitClients(false);

	BSimpleMediaOutput* output = sProducer->BeginOutput();
	BSimpleMediaInput* input = sConsumer->BeginInput();

	output->SetAcceptedFormat(_BuildRawAudioFormat());
	input->SetAcceptedFormat(_BuildRawAudioFormat());

	assert(sConsumer->Connect(input, output) == B_OK);

	#ifdef DELAYED_MODE
	snooze(SNOOZE_FOR);
	#endif

	assert(sConsumer->Disconnect() == B_OK);

	_DeleteClients();
}


void _ProducerConsumerTest()
{
	_InitClients(false);

	BMediaOutput* output = sProducer->BeginOutput();
	BMediaInput* input = sConsumer->BeginInput();

	assert(sProducer->Connect(output, input) == B_OK);

	#ifdef DELAYED_MODE
	snooze(SNOOZE_FOR);
	#endif

	assert(sProducer->Disconnect() == B_OK);

	_DeleteClients();
}


void _ProducerFilterConsumerTest()
{
	_InitClients(true);

	BMediaOutput* output = sProducer->BeginOutput();
	BMediaInput* input = sConsumer->BeginInput();

	BMediaInput* filterInput = sFilter->BeginInput();
	BMediaOutput* filterOutput = sFilter->BeginOutput();

	assert(sFilter->Bind(filterInput, filterOutput) == B_OK);

	assert(sProducer->Connect(output, filterInput) == B_OK);
	assert(sFilter->Connect(filterOutput, input) == B_OK);
	#ifdef DELAYED_MODE
	snooze(SNOOZE_FOR);
	#endif

	assert(sFilter->Disconnect() == B_OK);

	_DeleteClients();
}


int main()
{
	printf("Testing Simple (1:1) Producer-Consumer configuration: ");
	_ConsumerProducerTest();
	_ProducerConsumerTest();
	printf("OK\n");

	printf("Testing Simple (1:1:1) Producer-Filter-Consumer configuration: ");
	_ProducerFilterConsumerTest();
	printf("OK\n");

	return 0;
}
